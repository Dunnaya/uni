import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.*;

public class Manager {
    private static final int PORT = 9999;
    private static final long TIMEOUT_MS = 10000;
    private static final long POLL_INTERVAL_MS = 200;

    public static void main(String[] args) throws Exception {
        System.out.println("\tCLIENT STARTED");
        
        // create scanner for user input
        @SuppressWarnings("resource")
        Scanner scanner = new Scanner(System.in);

        // ask user for the number x
        int x = 0;
        while (true) {
            System.out.print("Please enter integer number X: ");
            if (scanner.hasNextInt()) {
                x = scanner.nextInt();
                scanner.nextLine(); // consume the leftover newline after the number
                break;
            } else {
                System.out.println("Error: That's not a number. Try again.");
                scanner.nextLine(); // clear the buffer
            }
        }

        System.out.println("Calculating f(x) * g(x) for x = " + x);
        System.out.println("---------------------------------------");
        System.out.println(">>> Press 'q' and Enter at any time to CANCEL computation. <<<");

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            
            ExecutorService executor = Executors.newFixedThreadPool(2);
            List<Process> processes = new ArrayList<>();

            // to pass x into the lambda it must be effectively final so we copy it
            int finalX = x;

            // start Worker1
            CompletableFuture<Double> future1 = CompletableFuture.supplyAsync(() -> {
                try {
                    Process p = startWorkerProcess(1, finalX);
                    synchronized(processes) { processes.add(p); }
                    return receiveResult(serverSocket);
                } catch (Exception e) { throw new RuntimeException(e); }
            }, executor);

            // start Worker2
            CompletableFuture<Double> future2 = CompletableFuture.supplyAsync(() -> {
                try {
                    Process p = startWorkerProcess(2, finalX);
                    synchronized(processes) { processes.add(p); }
                    return receiveResult(serverSocket);
                } catch (Exception e) { throw new RuntimeException(e); }
            }, executor);

            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                for (Process p : processes) p.destroy();
            }));

            long startTime = System.currentTimeMillis();
            Double res1 = null;
            Double res2 = null;
            boolean cancelled = false;

            // main loop
            while (!future1.isDone() || !future2.isDone()) {
                
                // check timeout
                if (System.currentTimeMillis() - startTime > TIMEOUT_MS) {
                    System.out.println("\n[!] ERROR: Computation timed out!");
                    cancelled = true;
                    break;
                }

                // check for 'q' key press
                if (System.in.available() > 0) {
                    String input = scanner.nextLine();
                    if (input.trim().equalsIgnoreCase("q")) {
                        System.out.println("\n[!] USER CANCELLED THE OPERATION.");
                        cancelled = true;
                        break;
                    }
                }

                // hard stop check
                if (future1.isDone() && res1 == null) {
                    res1 = future1.get(); 
                    if (res1 == 0.0) {
                        System.out.println("\n[!] Worker 1 returned 0.0. HARD STOP initiated.");
                        cancelled = true; 
                        res2 = 0.0; 
                        break; 
                    }
                }
                if (future2.isDone() && res2 == null) {
                    res2 = future2.get();
                    if (res2 == 0.0) {
                        System.out.println("\n[!] Worker 2 returned 0.0. HARD STOP initiated.");
                        cancelled = true;
                        res1 = 0.0;
                        break;
                    }
                }

                System.out.print("."); 
                Thread.sleep(POLL_INTERVAL_MS);
            }

            System.out.println(); 

            if (cancelled) {
                future1.cancel(true);
                future2.cancel(true);
                if ((res1 != null && res1 == 0.0) || (res2 != null && res2 == 0.0)) {
                     System.out.println(">>> FINAL RESULT: 0.0 (Computed via Hard Stop)");
                } else {
                     System.out.println(">>> OPERATION ABORTED BY USER OR TIMEOUT.");
                }
            } else {
                res1 = future1.get();
                res2 = future2.get();
                System.out.println("Worker 1 Result: " + res1);
                System.out.println("Worker 2 Result: " + res2);
                System.out.println(">>> FINAL RESULT: " + (res1 * res2));
            }

            for (Process p : processes) p.destroy();
            executor.shutdownNow();
        }
    }

    private static Process startWorkerProcess(int funcId, int x) throws IOException {
        String javaHome = System.getProperty("java.home");
        String javaBin = javaHome + File.separator + "bin" + File.separator + "java";
        String classpath = System.getProperty("java.class.path");
        
        ProcessBuilder builder = new ProcessBuilder(
                javaBin, "-cp", classpath, "Worker", 
                String.valueOf(PORT), String.valueOf(funcId), String.valueOf(x));
        builder.redirectError(ProcessBuilder.Redirect.INHERIT);
        return builder.start();
    }

    private static Double receiveResult(ServerSocket server) throws IOException, ClassNotFoundException {
        try (Socket client = server.accept();
             ObjectInputStream in = new ObjectInputStream(client.getInputStream())) {
            return (Double) in.readObject();
        }
    }
}