import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.*;

public class Manager {
    // communication port
    private static final int PORT = 9999;

    public static void main(String[] args) throws Exception {
        // number x we are processing
        int x = 5; 
        System.out.println("Start. X = " + x);

        // server socket waits for connections from workers
        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            
            // thread pool for asynchrony
            ExecutorService executor = Executors.newFixedThreadPool(2);
            List<Process> processes = new ArrayList<>();

            // 1. launch task 1 (Worker 1) asynchronously
            CompletableFuture<Double> future1 = CompletableFuture.supplyAsync(() -> {
                try {
                    // launch a separate Java process
                    Process p = startWorkerProcess(1, x);
                    processes.add(p);
                    return receiveResult(serverSocket);
                } catch (Exception e) { throw new RuntimeException(e); }
            }, executor);

            // 2. launch task 2 (Worker 2) asynchronously
            CompletableFuture<Double> future2 = CompletableFuture.supplyAsync(() -> {
                try {
                    Process p = startWorkerProcess(2, x);
                    processes.add(p);
                    return receiveResult(serverSocket);
                } catch (Exception e) { throw new RuntimeException(e); }
            }, executor);

            // 3. processing logic (small exception cases from the video)
            try {
                // wait for results (or timeout 5 seconds)
                // anyOf is not suitable because we need both, but acceptEither can be used for 'who is first'
                // here we wait for both:
                Double res1 = future1.get(5, TimeUnit.SECONDS);
                Double res2 = future2.get(5, TimeUnit.SECONDS);

                System.out.println("Result 1: " + res1);
                System.out.println("Result 2: " + res2);
                
                // final aggregation (multiplication as an ex.)
                System.out.println("Final Result (Multiply): " + (res1 * res2));

            } catch (TimeoutException e) {
                System.out.println("TIMEOUT! Cancelling tasks...");
                future1.cancel(true);
                future2.cancel(true);
            } catch (Exception e) {
                System.out.println("Error occurred: " + e.getMessage());
            } finally {
                // kill worker processes on exit
                for (Process p : processes) p.destroy();
                executor.shutdown();
                System.out.println("Done.");
            }
        }
    }

    // method to start an external Java process
    private static Process startWorkerProcess(int funcId, int x) throws IOException {
        String javaHome = System.getProperty("java.home");
        String javaBin = javaHome + File.separator + "bin" + File.separator + "java";
        String classpath = System.getProperty("java.class.path");
        
        ProcessBuilder builder = new ProcessBuilder(
                javaBin, "-cp", classpath, "Worker", 
                String.valueOf(PORT), String.valueOf(funcId), String.valueOf(x));
        
        // redirect input/output (to see worker errors in manager console)
        builder.inheritIO(); 
        return builder.start();
    }

    // method to receive result via socket
    private static Double receiveResult(ServerSocket server) throws IOException, ClassNotFoundException {
        try (Socket client = server.accept();
             ObjectInputStream in = new ObjectInputStream(client.getInputStream())) {
            return (Double) in.readObject();
        }
    }
}