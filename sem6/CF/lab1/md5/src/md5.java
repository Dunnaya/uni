import java.util.Scanner;
import java.nio.charset.StandardCharsets;

public class md5 {

    // shift constants for each of 64 operations
    private static final int[] S = {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };

    // T[i] = floor(2^32 * abs(sin(i+1)))
    private static final int[] T = buildT();
 
    private static int[] buildT() {
        int[] t = new int[64];
        for (int i = 0; i < 64; i++) {
            t[i] = (int)(long)(Math.abs(Math.sin(i + 1)) * 4294967296L);
        }
        return t;
    }

    public static String computeMD5(String input) {
        // lil fix: need explicit UTF-8, otherwise results differ on windows vs linux
        byte[] msg = input.getBytes(StandardCharsets.UTF_8);
        int msgLen = msg.length;

        // padding / length must be 448 bits (56 bytes) modulo 512 (64 bytes) / append 0x80, then zeros, then 8-byte length
        int numBlocks = ((msgLen + 8) >>> 6) + 1;
        int totalLen = numBlocks << 6;
        
        byte[] paddBytes = new byte[totalLen - msgLen];
        
        paddBytes[0] = (byte) 0x80;

        long msgLenBits = (long) msgLen << 3;
        for (int i = 0; i < 8; i++) {
            paddBytes[paddBytes.length - 8 + i] = (byte) (msgLenBits >>> (8 * i));
        }

        byte[] buf = new byte[totalLen];
        System.arraycopy(msg, 0, buf, 0, msgLen);
        System.arraycopy(paddBytes, 0, buf, msgLen, paddBytes.length);

        // initial hash values from RFC 1321
        int a = 0x67452301;
        int b = 0xefcdab89;
        int c = 0x98badcfe;
        int d = 0x10325476;

        for (int i = 0; i < numBlocks; i++) {
            int[] x = new int[16];
            for (int j = 0; j < 16; j++) {
                x[j] = (buf[i * 64 + j * 4] & 0xFF) |
                       ((buf[i * 64 + j * 4 + 1] & 0xFF) << 8) |
                       ((buf[i * 64 + j * 4 + 2] & 0xFF) << 16) |
                       ((buf[i * 64 + j * 4 + 3] & 0xFF) << 24);
            }

            int aa = a, bb = b, cc = c, dd = d;

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
                b = b + Integer.rotateLeft(a + f + T[j] + x[g], S[j]);
                a = temp;
            }

            a += aa;
            b += bb;
            c += cc;
            d += dd;
        }

        // little-endian hex output
        return toHexStr(a) + toHexStr(b) + toHexStr(c) + toHexStr(d);
    }

    // MD5 outputs bytes in little-endian order so need to reverse
    private static String toHexStr(int val) {
        return String.format("%08x", Integer.reverseBytes(val));
    }
    public static void main(String[] args) {
        // sanity check =))
        // "" -> d41d8cd98f00b204e9800998ecf8427e
        // "abc" -> 900150983cd24fb0d6963f7d28e17f72
        System.out.println(computeMD5(""));
        System.out.println(computeMD5("abc"));
 
        Scanner sc = new Scanner(System.in);
        System.out.print("Enter string: ");
        String input = sc.nextLine();
        System.out.println("MD5: " + computeMD5(input));
    }
}