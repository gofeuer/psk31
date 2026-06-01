#include "psk31.h"
#include "varicode.h"

static struct {
    unsigned short varicode;
    unsigned short zero_shift;
    ascii_callback emit;
} decoder;

static void process_bit(unsigned char bit) {
    if(bit) { // 1
        decoder.varicode <<= decoder.zero_shift;
        decoder.varicode |= 1;
        decoder.zero_shift = 1;
        return;
    }

    if (++decoder.zero_shift > 2) { // 00
        if (decoder.varicode && decoder.varicode <= LARGEST_VALID_VARICODE) {
            decoder.emit(ascii_table[decoder.varicode]);
        }
        decoder.varicode = 0;
    }
}

void decoder_init(ascii_callback callback) {
    decoder.varicode = 0;
    decoder.zero_shift = 1;
    decoder.emit = callback;

    // Initialize 'ascii_table' with the space character.
    for (int i = 0; i < LARGEST_VALID_VARICODE + 1; i++) {
        ascii_table[i] = ' ';
    }

    // Unpack 'ascii_table_packed' into 'ascii_table'.
    for (int i = 0; i < 128; i++) {
        varicode_ascii packed = ascii_table_packed[i];
        ascii_table[packed.varicode] = packed.ascii;
    }
}

void decoder_push(unsigned char byte) {
    for(int i = 0; i < 8; i++, byte <<= 1) {
        process_bit(byte & 0x80);
    }
}
