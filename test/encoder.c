#include "../psk31.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Hello World
//     H        e       l        l       o     -        W         o       r        l        d
// 101010101 00 11 00 11011 00 11011 00 111 00 1 00 101011101 00 111 00 10101 00 11011 00 101101

static int test_start(void) {
    int pass = 0;

    uint16_t buffer[8];
    memset(buffer, -1, sizeof(uint16_t) * 8); // A "dirty" buffer

    // Initializing just the first value is enough
    encoder_start(buffer);
    pass += !(buffer[0] == 0); // 0000 0000:0000 0000
    
    return pass;
}

static int test_push(void) {
    int pass = 0;

    {
        uint16_t buffer[8];
        encoder_start(buffer);

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
        pass += !(buffer[0] == 0x99AA); // 1001 1001:1010 1010
        pass += !(buffer[1] == 0x67B3); // 0110 0111:1011 0011
        pass += !(buffer[2] == 0x7425); // 0111 0100:0010 0101
        pass += !(buffer[3] == 0x4DE5); // 0100 1101:1110 0101
        // Little-endian 
        pass += !(buffer[4] == 0x9680); // 1001 0110:1000 0000
    }
    {   // Test word break on character separator
        uint16_t buffer[8];
        encoder_start(buffer);

        encoder_push('Z'); // 1010101101 00
        encoder_push('o'); // 111 00
        encoder_push('o'); // 111

        // Big-endian
        pass += !(buffer[0] == 0x4EAB); // 0100 1110:1010 1011
        // Little-endian 
        pass += !(buffer[1] == 0x7000); // 0111 0000:0000 0000
    }

    return pass;
}

static int test_done(void) {
    int pass = 0;
    uint16_t buffer[8];
    uint8_t *stream;
    uint16_t length;
    
    encoder_start(buffer);

    encoder_push('G'); // 11111101 00
    encoder_push('o'); // 111

    length = encoder_done(&stream);

    // Big-endian
    pass += !(stream == (uint8_t *)buffer);
    pass += !(length == 2);

    pass += !(buffer[0] == 0x38FD); // 0011 1000:1111 1101

    pass += !(stream[0] == 0xFD); // 1111 1101
    pass += !(stream[1] == 0x38); // 0011 1000

    return pass;
}

static int test_all(void) {
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
