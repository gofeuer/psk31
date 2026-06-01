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
static void vacant_push(varicode varicode) {
    encoder.buffer[encoder.index] |= varicode.encoded_bits << (encoder.bits_free - varicode.bit_count);
    encoder.bits_free -= varicode.bit_count + VARICODE_LETTER_GAP;
}

// ( cramped )
static void cramped_push(varicode varicode) {
    int overflow = varicode.bit_count - encoder.bits_free;
    
    encoder.buffer[encoder.index] |= varicode.encoded_bits >> overflow;
    swap_bytes(&encoder.buffer[encoder.index]); // Flip endianness for transmission.
    encoder.buffer[++encoder.index] = varicode.encoded_bits << (16 - overflow); // Write the overflowed bits to the next index.

    // Keep track of how much space is left in this new 'encoder.buffer[encoder.index]'
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

    varicode varicode = varicode_table[ascii];

    if (encoder.bits_free < varicode.bit_count) { // Is there enough space at the current buffer position?
        cramped_push(varicode); // No.
    } else {
        vacant_push(varicode);  // Yes.
    }
}

// ( vacant|cramped ) -- done -->o
int encoder_done(void) {
    swap_bytes(&encoder.buffer[encoder.index]); // Flip the last encoded word.
    return (encoder.index + 1) * 2; // Return the length of the stream in bytes.
}
