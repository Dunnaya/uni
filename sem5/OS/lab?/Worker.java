import java.io.*;
import java.net.Socket;

public class Worker {
    public static void main(String[] args) {
        if (args.length < 3) return;
        
        int port = Integer.parseInt(args[0]);
        int functionId = Integer.parseInt(args[1]);
        int x = Integer.parseInt(args[2]);

        // artificial failure for testing
        if (x == 2) {
            System.err.println("CRITICAL ERROR: Artificial Fail triggered!");
            System.exit(1); // dies immediately
        }

        try (Socket socket = new Socket("localhost", port);
             ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream())) {

            double result = compute(functionId, x);
            out.writeObject(result);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static double compute(int id, int x) throws InterruptedException {
        if (id == 1) {
            // f(x) = x^2
            Thread.sleep(2000); 
            return Math.pow(x, 2);
        } else {
            // g(x) = (x - 10)^2
            Thread.sleep(4000); 
            return Math.pow(x - 10, 2);
        }
    }
}