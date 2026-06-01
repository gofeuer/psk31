# PSK31 Codec

A compact, embedded-friendly ASCII-to-Varicode encoder and decoder for PSK31.

## Overview

PSK31 is a digital mode used by amateur radio operators. This library encodes ASCII text into Varicode bit sequences and decodes them back. The implementation is designed for low-powered and embedded processors with no dynamic memory allocation.

## Usage

**Encoding:** Convert ASCII to Varicode bit sequences. Each character becomes a variable-length sequence of 1s and 0s, terminated by at least two consecutive 0s.

```c
unsigned char buffer[16];
encoder_start(buffer);
encoder_push('H');
encoder_push('i');
int bytes = encoder_done();
// buffer contains encoded bits, bytes indicates length
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

The decoder maintains state: the current Varicode value being assembled and a count of consecutive zeros. When `zero_count >= 2`, the accumulated value is looked up in the Varicode table and emitted via callback.

## Repository layout

- `encoder.c` — encoder implementation
- `decoder.c` — decoder implementation
- `psk31.h` — public API
- `varicode.h` — Varicode lookup tables
- `test/` — encoder and decoder tests

## Design

- No dynamic memory allocation
- Caller-provided buffers
- Portable C implementation
- Callback-based decoder for streaming processing
