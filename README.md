# PSK31 Codec

A compact, embedded-friendly ASCII-to-Varicode encoder and decoder for PSK31, specifically optimized for the **Intel 8086 (16-bit) processor**.

## Overview

PSK31 is a digital mode used by amateur radio operators. This library encodes ASCII text into Varicode bit sequences and decodes them back. The implementation is designed for low-powered and embedded 16-bit processors with no dynamic memory allocation.

## Usage

**Encoding:** Convert ASCII to Varicode bit sequences. Each character becomes a variable-length sequence of 1s and 0s, terminated by at least two consecutive 0s. The encoder processes data in 16-bit words and automatically handles endianness swapping (from little-endian to big-endian) for transmission.

```c
unsigned char buffer[256];
encoder_start(buffer);
encoder_push('H');
encoder_push('i');
int bytes = encoder_done();
// buffer contains encoded bits, bytes indicates stream length
```

**Decoding:** Reverse the process by accumulating bits until the character separator (two consecutive 0s) is detected, then emit the character.

```c
void on_char(char c) {
    printf("%c", c);
}

decoder_init(on_char);
decoder_push(encoded_byte_1);
decoder_push(encoded_byte_2);
// on_char() called for each decoded character
```

The decoder maintains state: the current Varicode value being assembled and a count of consecutive zeros. When `zero_count >= 2`, the accumulated value is looked up in the sparse Varicode table and emitted via callback.

## Repository layout

- `encoder.c` — encoder implementation (optimized for 16-bit architecture)
- `decoder.c` — decoder implementation
- `psk31.h` — public API
- `varicode.h` — Varicode lookup tables (including packed sparse table)
- `test/` — encoder and decoder tests

## Design

- Exclusive optimizations for 16-bit processors (Intel 8086)
- 16-bit word buffering and endianness swapping for transmission
- Sparse reverse lookup table for memory-efficient decoding
- No dynamic memory allocation
- Caller-provided buffers
- Portable C implementation
- Callback-based decoder for streaming processing
