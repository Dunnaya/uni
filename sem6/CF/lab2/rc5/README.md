# RC5 block cipher in Java

From-scratch implementation of the RC5-32/12/16 symmetric block cipher in pure Java.

## Parameters

| Parameter | Value | Meaning |
|-----------|-------|---------|
| W | 32 | Word size in bits |
| R | 12 | Number of rounds |
| B | 16 | Key length in bytes |

Block size = `2W` = 64 bits (8 bytes), expanded key table = `2(R+1)` = 26 words

## How it works

**1. Key Expansion**  
The key is loaded into array `L` as little-endian 32-bit words, then a subkey table `S` is initialized from two constants — `P32` and `Q32` — which come from the fractional parts of *e* and the golden ratio. After that, `S` and `L` are mixed together over `3 * max(T, c)` iterations.

**2. Encryption**  
The 8-byte block splits into two 32-bit words `A` and `B`. After adding the first two subkeys, 12 rounds run:
```
A = rotL(A ^ B, B) + S[2i]
B = rotL(B ^ A, A) + S[2i+1]
```
The rotation amount depends on the current data — that's the main thing that makes RC5 different from simpler ciphers.

**3. Decryption**  
Same rounds in reverse, right rotations, subkeys subtracted instead of added.

## Usage

```
javac RC5.java
java RC5
```

```
encrypted: 55f5ad8fc5e5c7df
decrypted: testdata

Enter 8-char string to encrypt: hellwrld
encrypted: d536ba89d9fa0a3b
decrypted: hellwrld
```

## Implementation notes

- `ByteBuffer` defaults to big-endian — had to set `ByteOrder.LITTLE_ENDIAN` explicitly, otherwise the output is just wrong (took me a bit to figure out).
- Ceiling division `(KB + u - 1) / u` in key expansion instead of plain `KB / u` — avoids dropping the last bytes if key length isn't a multiple of word size.
- `y &= W - 1` in rotations keeps the shift in `[0, 31]` as the spec requires.

## Limitations

Works only with exactly 8-byte blocks. To handle arbitrary input you'd need padding (e.g. PKCS#7) and a block mode like CBC or CTR.

## ts pmo