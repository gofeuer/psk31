# PSK31 Encoder

A compact, embedded-friendly ASCII-to-Varicode encoder for PSK31.

## Overview

PSK31 is a digital mode used by amateur radio operators. This repository contains a small C encoder that converts ASCII characters into Varicode bit patterns suitable for PSK31-style transmission.

The implementation is specifically designed for low-powered and embedded processors. It avoids dynamic memory allocation and operates on a fixed `uint16_t` buffer supplied by the caller.

## Features

- ASCII to Varicode encoding
- No heap allocation
- Fixed-size, caller-provided buffer
- Portable C implementation
- Simple encoder API

## Usage

1. Allocate a `uint16_t` buffer in the caller.
2. Call `encoder_start(buffer)` to initialize encoding.
3. Call `encoder_push(c)` for each ASCII character.
4. Call `encoder_done(&stream)` to finalize and obtain the byte stream.

After `encoder_done` returns:

- `stream` points to the same underlying bytes as the `uint16_t` buffer.
- The return value is the stream length in bytes.

## Repository layout

- `encoder.c` — encoder implementation
- `psk31.h` — public encoder API
- `varicode.h` — Varicode lookup table
- `test/encoder.c` — encoder tests for start/push/done behavior

## Design goals

- Minimal memory overhead
- No dynamic allocation
- Easy porting to embedded CPUs such as the 8086

## Build and test

A simple native build is available via the `Makefile` in the repository root. Example:

```sh
make
./test/bin/encoder
```

This project is intended to become part of a broader 8086-targeted radio project. The current implementation is a portable C prototype.

## Notes

- The caller must provide a buffer large enough for the encoded message.
- `encoder_done` returns the number of bytes in the encoded stream.
- The encoded output uses the same memory buffer as the input, exposed as `uint8_t *`.

## Roadmap

- Add 8086 cross-compilation instructions
- Verify behavior on 8086 toolchains
- Extend the encoder for full PSK31 framing and transmission

