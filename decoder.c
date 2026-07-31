#include "psk31.h"
#include "varicode.h"

static struct {
    unsigned short varicode;
    unsigned short zero_shift;
    ascii_callback emit;
} decoder;

static inline void process_bit(unsigned char bit) {
    if(bit) { // 1
        if (decoder.zero_shift) decoder.varicode <<= 1;
        decoder.varicode <<= 1;
        decoder.varicode |= 1;
        decoder.zero_shift = 0;
        return;
    }

    if (decoder.varicode == 0) return;

    if (++decoder.zero_shift == 2) { // 00 gap   
        for (unsigned char ascii_char = 0; ascii_char < 128; ascii_char++) {
            if (varicode_table[ascii_char] == decoder.varicode) {
                decoder.emit(ascii_char);
                break;
            }
        }
        decoder.varicode = 0;
        decoder.zero_shift = 0;
    }
}

void decoder_init(ascii_callback callback) {
    decoder.varicode = 0;
    decoder.zero_shift = 0;
    decoder.emit = callback;
}

void decoder_push(unsigned char byte) {
    for(int i = 0; i < 8; i++, byte <<= 1) {
        process_bit((byte << 1) >> 8);
    }
}
