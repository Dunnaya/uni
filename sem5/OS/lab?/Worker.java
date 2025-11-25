import java.io.*;
import java.net.Socket;

public class Worker {
    public static void main(String[] args) {
        // args: port, function id (1 or 2), x value
        if (args.length < 3) return;
        
        int port = Integer.parseInt(args[0]);
        int functionId = Integer.parseInt(args[1]);
        int x = Integer.parseInt(args[2]);

        try (Socket socket = new Socket("localhost", port);
             ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream())) {

            // complicated computation based on functionId
            double result = compute(functionId, x);
            
            // send result back to Manager
            out.writeObject(result);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static double compute(int id, int x) throws InterruptedException {
        // here we simulate different functions
        if (id == 1) {
            // function 1: a bit longer (e.g., x * 2)
            Thread.sleep(2000); // sleep for 2 sec
            return x * 2.0;
        } else {
            // function 2: (e.g., x ^ 2)
            Thread.sleep(1000); // sleep for 1 sec
            // return 0.0; // uncomment to simulate hard case
            return x * x * 1.0;
        }
    }
}