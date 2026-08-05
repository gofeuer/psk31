#include "psk31.h"
#define PSK31_VARICODE_TABLE
#include "varicode.h"

static struct {
    unsigned short *buffer;
    unsigned int index;
    signed char bits_free;
} encoder;

static inline unsigned short swap_bytes(unsigned short val) {
    return (val << 8) | (val >> 8);
}

// ( vacant )
static inline void vacant_push(unsigned char bit_count, unsigned short encoded_bits) {
    encoder.buffer[encoder.index] |= encoded_bits << (encoder.bits_free - bit_count);
    encoder.bits_free -= bit_count + VARICODE_LETTER_GAP;
}

// ( cramped )
static inline void cramped_push(unsigned char bit_count, unsigned short encoded_bits) {
    unsigned char overflow = bit_count - encoder.bits_free;
    
    encoder.buffer[encoder.index] |= encoded_bits >> overflow;
    encoder.buffer[encoder.index] = swap_bytes(encoder.buffer[encoder.index]); // Flip endianness for transmission.
    encoder.buffer[++encoder.index] = encoded_bits << (16 - overflow); // Write the overflowed bits to the next index.

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
    unsigned char ascii_char = (unsigned char)ascii;
    if (ascii_char > 127) return; // Invalid ASCII character, ignore it.

    unsigned short encoded_bits = varicode_table[ascii_char];
    unsigned char bit_count = get_bit_count(encoded_bits);

    if (encoder.bits_free < bit_count) { // Is there enough space at the current buffer position?
        cramped_push(bit_count, encoded_bits); // No.
    } else {
        vacant_push(bit_count, encoded_bits); // Yes.
    }
}

// ( vacant|cramped ) -- done -->o
int encoder_done(void) {
    encoder.buffer[encoder.index] = swap_bytes(encoder.buffer[encoder.index]); // Flip the last encoded word.
    return (encoder.index + 1) * 2; // Return the length of the stream in bytes.
}
