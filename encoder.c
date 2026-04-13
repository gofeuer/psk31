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
    *word = ((*word << 8) | (*word >> 8));
}

// ( varicode [vacant] )
static inline void stream_vacant(uint16_t varicode_char, const uint16_t varicode_bitCount) {
    uint16_t shift = (varicode.bitsFree - varicode_bitCount);
    varicode.buffer[varicode.index] |= (varicode_char << shift);

    varicode.bitsFree -= (varicode_bitCount + VARICODE_LETTER_GAP);
}

// ( varicode [cramped] )
static inline void stream_cramped(uint16_t varicode_char, const uint16_t varicode_bitCount) {
    uint16_t shift = (varicode_bitCount - varicode.bitsFree);
    varicode.buffer[varicode.index] |= (varicode_char >> shift);
    // 'varicode.buffer[varicode.index]' is now full

    // Flip endianness for transmission, then address the next slot
    swap_bytes(&varicode.buffer[varicode.index]);
    ++varicode.index; // Maybe tell the compiler to PUSH 'word' into a stack?

    shift = ((16 - varicode_bitCount) + varicode.bitsFree);
    varicode.buffer[varicode.index] = (varicode_char << shift);

    varicode.bitsFree = (shift - VARICODE_LETTER_GAP);
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
uint16_t encoder_done(void) {
    // 'encoder_push(...)' doesn't flip the last address 
    swap_bytes(&varicode.buffer[varicode.index]);
    return varicode.index;
}
