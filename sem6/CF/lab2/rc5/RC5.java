import java.util.Scanner;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class RC5 {

    // RC5-32/12/16 params
    private static final int W = 32;
    private static final int R = 12;
    private static final int KB = 16; // key length in bytes
    private static final int T = 2 * (R + 1);

    // magic constants derived from e and golden ratio, specific to W = 32
    private static final int P32 = 0xb7e15163;
    private static final int Q32 = 0x9e3779b9;

    private final int[] S = new int[T];
 
    public RC5(byte[] key) {
        keyExpansion(key);
    }

    private void keyExpansion(byte[] key) {
        int u = W / 8; // bytes per word
        // use ceiling devision so we dont lose bytes when KB % u != 0
        int c = (KB + u - 1) / u;
        int[] L = new int[c];
        
        for (int i = 0; i < KB; i++) {
            L[i / u] |= (key[i] & 0xFF) << (8 * (i % u));
        }

        S[0] = P32;
        for (int i = 1; i < T; i++) {
            S[i] = S[i - 1] + Q32;
        }

        int i = 0, j = 0, A = 0, B = 0;
        int n = 3 * Math.max(T, c);

        for (int k = 0; k < n; k++) {
            A = S[i] = rotL(S[i] + A + B, 3);
            B = L[j] = rotL(L[j] + A + B, A + B);
            i = (i + 1) % T;
            j = (j + 1) % c;
        }
    }

    public byte[] encrypt(byte[] block) {
        // RC5 uses little-endian words / ByteBuffer is big-endian by default, must set order
        ByteBuffer buf = ByteBuffer.wrap(block).order(ByteOrder.LITTLE_ENDIAN);
        int A = buf.getInt() + S[0];
        int B = buf.getInt() + S[1];
 
        for (int i = 1; i <= R; i++) {
            A = rotL(A ^ B, B) + S[2 * i];
            B = rotL(B ^ A, A) + S[2 * i + 1];
        }
 
        return ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).putInt(A).putInt(B).array();
    }

    public byte[] decrypt(byte[] block) {
        ByteBuffer buf = ByteBuffer.wrap(block).order(ByteOrder.LITTLE_ENDIAN);
        int A = buf.getInt();
        int B = buf.getInt();
 
        for (int i = R; i >= 1; i--) {
            B = rotR(B - S[2 * i + 1], A) ^ A;
            A = rotR(A - S[2 * i], B) ^ B;
        }
 
        return ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).putInt(A - S[0]).putInt(B - S[1]).array();
    }

    // helper methods for bit rotation
    private int rotL(int x, int y) {
        y &= W - 1;
        return (x << y) | (x >>> (W - y));
    }
 
    private int rotR(int x, int y) {
        y &= W - 1;
        return (x >>> y) | (x << (W - y));
    }

    private static String toHex(byte[] bytes) {
        StringBuilder sb = new StringBuilder();
        for (byte b : bytes) sb.append(String.format("%02x", b));
        return sb.toString();
    }
 
    public static void main(String[] args) {
        // key must be exactly 16 bytes (KB), block must be exactly 8 bytes
        byte[] key   = "secretpassword12".getBytes();
        byte[] plain = "testdata".getBytes();
 
        RC5 cipher = new RC5(key);
 
        byte[] enc = cipher.encrypt(plain);
        System.out.println("encrypted: " + toHex(enc));
 
        byte[] dec = cipher.decrypt(enc);
        System.out.println("decrypted: " + new String(dec));
 
        // interactive
        Scanner sc = new Scanner(System.in);
        System.out.print("\nEnter 8-char string to encrypt: ");
        String input = sc.nextLine();
        if (input.length() != 8) {
            System.out.println("Must be exactly 8 characters");
            return;
        }
        byte[] encInput = cipher.encrypt(input.getBytes());
        System.out.println("encrypted: " + toHex(encInput));
        System.out.println("decrypted: " + new String(cipher.decrypt(encInput)));
    }
}