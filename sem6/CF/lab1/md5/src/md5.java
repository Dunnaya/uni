import java.util.Scanner;

public class md5 {

    // shift constants for each step (S-box)
    private static final int[] S = {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };

    // table of constants T (integer part of 2^32 * abs(sin(i+1)))
    private static final int[] T = new int[64];
    static {
        for (int i = 0; i < 64; i++) {
            T[i] = (int) (long) (Math.abs(Math.sin(i + 1)) * 4294967296L);
        }
    }

    public static String computeMD5(String input) {
        byte[] message = input.getBytes();
        int messageLenBytes = message.length;

        // 1) padding
        // length must be 448 bits (56 bytes) modulo 512 (64 bytes)
        int numBlocks = ((messageLenBytes + 8) >>> 6) + 1;
        int totalLen = numBlocks << 6;
        
        byte[] paddingBytes = new byte[totalLen - messageLenBytes];
        
        paddingBytes[0] = (byte) 0x80; // add a single bit

        // add the message length (64 bits) at the end (Little-endian)
        long messageLenBits = (long) messageLenBytes << 3;
        for (int i = 0; i < 8; i++) {
            paddingBytes[paddingBytes.length - 8 + i] = (byte) (messageLenBits >>> (8 * i));
        }

        // combining all into one array
        byte[] buffer = new byte[totalLen];
        System.arraycopy(message, 0, buffer, 0, messageLenBytes);
        System.arraycopy(paddingBytes, 0, buffer, messageLenBytes, paddingBytes.length);

        // 2) register initialization (MD5 magic constants)
        int a = 0x67452301;
        int b = 0xefcdab89;
        int c = 0x98badcfe;
        int d = 0x10325476;

        // 3) main processing loop
        for (int i = 0; i < numBlocks; i++) {
            int[] x = new int[16]; // block split into 16 words of 32 bits
            for (int j = 0; j < 16; j++) {
                x[j] = ((buffer[i * 64 + j * 4] & 0xFF)) |
                       ((buffer[i * 64 + j * 4 + 1] & 0xFF) << 8) |
                       ((buffer[i * 64 + j * 4 + 2] & 0xFF) << 16) |
                       ((buffer[i * 64 + j * 4 + 3] & 0xFF) << 24);
            }

            int originalA = a, originalB = b, originalC = c, originalD = d;

            // 64 iterations (4 rounds of 16 steps)
            for (int j = 0; j < 64; j++) {
                int f, g;
                if (j < 16) {
                    f = (b & c) | (~b & d);
                    g = j;
                } else if (j < 32) {
                    f = (d & b) | (~d & c);
                    g = (5 * j + 1) % 16;
                } else if (j < 48) {
                    f = b ^ c ^ d;
                    g = (3 * j + 5) % 16;
                } else {
                    f = c ^ (b | ~d);
                    g = (7 * j) % 16;
                }

                int temp = d;
                d = c;
                c = b;
                int sum = a + f + T[j] + x[g];
                b = b + Integer.rotateLeft(sum, S[j]);
                a = temp;
            }

            // add the result to the previous values
            a += originalA;
            b += originalB;
            c += originalC;
            d += originalD;
        }

        // 4) formatting the result in Hex
        return toHexString(a) + toHexString(b) + toHexString(c) + toHexString(d);
    }

    private static String toHexString(int b) {
        // MD5 requires Little-endian representation in Hex
        int reverse = Integer.reverseBytes(b);
        return String.format("%08x", reverse);
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.print("Enter string: ");
        String input = sc.nextLine();
        
        String result = computeMD5(input);
        System.out.println("MD5 hash: " + result);
    }
}