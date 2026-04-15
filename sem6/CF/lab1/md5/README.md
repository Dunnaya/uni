# MD5 hash in Java

From-scratch implementation of the MD5 hashing algorithm in pure Java.

## What this is

MD5 (Message Digest 5) takes an arbitrary string and produces a fixed 128-bit (32 hex character) fingerprint. Even a tiny change in the input - one letter, one space — completely changes the output. That property is what makes hash functions useful for data integrity checks, password storage, and digital signatures (though MD5 specifically is no longer recommended for security-critical use).

This implementation follows the original specification from [RFC 1321](https://www.rfc-editor.org/rfc/rfc1321).

## How it works

The algorithm has four main stages:

**1. Padding**  
The input is padded so its length becomes 448 bits mod 512. A `0x80` byte is appended first, then zeros, then the original message length as a 64-bit little-endian integer. This ensures the total length is always a multiple of 512 bits (64 bytes).

**2. Initialization**  
Four 32-bit registers (`a`, `b`, `c`, `d`) are set to specific magic constants defined in the RFC. These are the starting state of the hash.

**3. Compression**  
The padded message is split into 64-byte blocks. Each block runs through 64 rounds of bitwise operations across four rounds:
- Round 1 (F): `(b & c) | (~b & d)`
- Round 2 (G): `(d & b) | (~d & c)`
- Round 3 (H): `b ^ c ^ d`
- Round 4 (I): `c ^ (b | ~d)`

Each round uses a different message word order and a precomputed constant from the sine table `T[i] = floor(2^32 * |sin(i+1)|)`. After 64 rounds, the block result is added back to the running state.

**4. Output**  
The four registers are serialized as little-endian hex strings and concatenated into the final 32-character digest.

## Usage (wow it's so non-obv)

```
javac MD5.java
java MD5
```

The program prints two known test vectors on startup, then prompts for input:

```
d41d8cd98f00b204e9800998ecf8427e   ← hash of ""
900150983cd24fb0d6963f7d28e17f72   ← hash of "abc"
Enter string: helloworld
MD5: fc5e038d38a57032085441e7fe7010b0
```

## Implementation notes

- Input strings are encoded as **UTF-8** explicitly. Without this, results can differ between Windows and Linux due to platform default encodings.
- The `T[]` table is built at class load time via `buildT()` rather than hardcoded — easier to verify against the RFC.
- Register copies before each block are named `aa, bb, cc, dd` — the same convention used in RFC 1321 pseudocode.
- Output uses `Integer.reverseBytes()` to handle little-endian byte ordering correctly.

## Why MD5 is broken (and why this still matters)

MD5 is considered cryptographically broken — collision attacks have been demonstrated, meaning two different inputs can produce the same hash. It should not be used for password hashing or digital signatures.

That said, understanding how MD5 works is a good foundation for learning about stronger hash functions like SHA-256, which follow the same general structure but with more rounds and a larger state.

## Сенкс фор йо аттеншн, хев е найс дей =))