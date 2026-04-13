#include "../psk31.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Hello World
//     H        e       l        l       o     -        W         o       r        l        d
// 101010101 00 11 00 11011 00 11011 00 111 00 1 00 101011101 00 111 00 10101 00 11011 00 101101

static int test_start() {
    int pass = 0;

    uint16_t varicode_buffer[8];
    memset(varicode_buffer, -1, sizeof(uint16_t) * 8); // A "dirty" buffer

    // Initializing just the first value is enough
    encoder_start(varicode_buffer);
    pass += !(varicode_buffer[0] == 0b0000000000000000);
    
    return pass;
}

static int test_push() {
    int pass = 0;

    {
        uint16_t varicode_buffer[8];
        encoder_start(varicode_buffer);

        encoder_push('H');
        encoder_push('e');
        encoder_push('l');
        encoder_push('l');
        encoder_push('o');
        encoder_push(' ');
        encoder_push('W');
        encoder_push('o');
        encoder_push('r');
        encoder_push('l');
        encoder_push('d');

        // Big-endian
        pass += !(varicode_buffer[0] == 0b1001100110101010);
        pass += !(varicode_buffer[1] == 0b0110011110110011);
        pass += !(varicode_buffer[2] == 0b0111010000100101);
        pass += !(varicode_buffer[3] == 0b0100110111100101);
        // Little-endian 
        pass += !(varicode_buffer[4] == 0b1001011010000000);
    }
    {   // Test word break on character separator
        uint16_t varicode_buffer[8];
        encoder_start(varicode_buffer);

        encoder_push('Z'); // 1010101101 00
        encoder_push('o'); // 111 00
        encoder_push('o'); // 111

        // Big-endian
        pass += !(varicode_buffer[0] == 0b0100111010101011);
        // Little-endian 
        pass += !(varicode_buffer[1] == 0b0111000000000000);
    }

    return pass;
}

static int test_done() {
    int pass = 0;
    uint16_t varicode_buffer[8];
    encoder_start(varicode_buffer);

    encoder_push('G'); // 11111101 00
    encoder_push('o'); // 111

    uint16_t index = encoder_done();

    // Big-endian
    pass += !(varicode_buffer[0] == 0b0011100011111101);
    pass += !(index == 0);

    return pass;
}

static int test_all() {
    return 
        test_start() |
        test_push()  |
        test_done();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Running: all tests\n");
        return test_all();
    } else if (strcmp(argv[1], "start") == 0) {
        printf("Running: %s\n", argv[1]);
        return test_start();
    } else if (strcmp(argv[1], "push") == 0) {
        printf("Running: %s\n", argv[1]);
        return test_push();
    } else if (strcmp(argv[1], "done") == 0) {
        printf("Running: %s\n", argv[1]);
        return test_done();
    } else {
        printf("Unknown test: %s\n", argv[1]);
        return 1;
    }
}
