#include "psk31.h"
#include "varicode.h"
#include <stdint.h>

#define VARICODE_LETTER_GAP 2 // 00

static struct {
    uint16_t *buffer;
    uint16_t index;
    int16_t bitsFree;
} varicode;

static inline void swap_bytes(uint16_t *word) {
    uint16_t value = *word;
    *word = (value << 8) | (value >> 8);
}

// ( varicode [vacant] )
static inline void stream_vacant(uint16_t varicode_char, const uint16_t varicode_bitCount) {
    uint16_t shift = (varicode.bitsFree - varicode_bitCount);
    varicode.buffer[varicode.index] |= (varicode_char << shift);

    varicode.bitsFree -= (varicode_bitCount + VARICODE_LETTER_GAP);
}

// ( varicode [cramped] )
static inline void stream_cramped(uint16_t varicode_char, const uint16_t varicode_bitCount) {
    uint16_t overflow = (varicode_bitCount - varicode.bitsFree);
    varicode.buffer[varicode.index] |= (varicode_char >> overflow);
    // 'varicode.buffer[varicode.index]' is now full

    // Flip endianness for transmission, then address the next slot
    swap_bytes(&varicode.buffer[varicode.index]);
    ++varicode.index; // Maybe tell the compiler to PUSH 'word' into a stack?

    varicode.buffer[varicode.index] = (varicode_char << (16 - overflow));
    varicode.bitsFree = 16 - overflow - VARICODE_LETTER_GAP;
}

//  o-- start --> ( varicode )
void encoder_start(uint16_t *buffer) {
    varicode.buffer = buffer;
    varicode.buffer[0] = 0b0000000000000000; // Initialized for the first 'push'

    varicode.index = 0;
    varicode.bitsFree = 16;
}

// ( varicode ) -- push --> ( varicode )
void encoder_push(const uint8_t ascii_char) {
    if (ascii_char > 127) return; // Invalid ASCII character, ignore it

    uint16_t varicode_bitCount = varicode_table[ascii_char][0];
    uint16_t varicode_char = varicode_table[ascii_char][1];

    // Is there enough space for this varicode at the current buffer position?
    if (varicode.bitsFree < varicode_bitCount) {
        stream_cramped(varicode_char, varicode_bitCount); // No
    } else {
        stream_vacant(varicode_char, varicode_bitCount);  // Yes
    }
}

// ( varicode ) -- done -->o
uint16_t encoder_done(uint8_t **stream) {
    // 'encoder_push(...)' doesn't flip the last buffered word
    swap_bytes(&varicode.buffer[varicode.index]);

    *stream = (uint8_t *)varicode.buffer;
    return ((varicode.index + 1) * 2); // Return the length of the stream in bytes
}
