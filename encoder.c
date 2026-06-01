#include "psk31.h"
#include "varicode.h"

static struct {
    unsigned short *buffer;
    int index;
    int bits_free;
} encoder;

static inline void swap_bytes(unsigned short *word) {
    *word = (*word << 8) | (*word >> 8);
}

// ( vacant )
static void vacant_push(int varicode_bit_count, unsigned short varicode_bits) {
    encoder.buffer[encoder.index] |= varicode_bits << (encoder.bits_free - varicode_bit_count);
    encoder.bits_free -= varicode_bit_count + VARICODE_LETTER_GAP;
}

// ( cramped )
static void cramped_push(int varicode_bit_count, unsigned short varicode_bits) {
    int overflow = varicode_bit_count - encoder.bits_free;
    
    encoder.buffer[encoder.index] |= varicode_bits >> overflow;
    swap_bytes(&encoder.buffer[encoder.index]); // Flip endianness for transmission.
    encoder.buffer[++encoder.index] = varicode_bits << (16 - overflow); // Write the overflowed bits to the next index.

    // Keep track of how much space is left in this new 'varicode.buffer[varicode.index]'
    encoder.bits_free = (16 - overflow) - VARICODE_LETTER_GAP;
}

//  o-- start --> ( vacant )
void encoder_start(unsigned char *buffer) {
    encoder.buffer = (unsigned short *)buffer;
    encoder.buffer[0] = 0; // Initialized for the first 'push' event.
    encoder.bits_free = 16; // All bits of the first position are free.
    encoder.index = 0;
}

// ( vacant|cramped ) -- push --> ( vacant|cramped )
void encoder_push(char ascii) {
    if ((unsigned char)ascii > 127) return; // Invalid ASCII character, ignore it.

    varicode code = varicode_table[(unsigned char)ascii];

    if (encoder.bits_free < code.bit_count) { // Is there enough space at the current buffer position?
        cramped_push(code.bit_count, code.encoded_bits); // No.
    } else {
        vacant_push(code.bit_count, code.encoded_bits);  // Yes.
    }
}

// ( vacant|cramped ) -- done -->o
int encoder_done(void) {
    swap_bytes(&encoder.buffer[encoder.index]); // Flip the last encoded word.
    return (encoder.index + 1) * 2; // Return the length of the stream in bytes.
}
