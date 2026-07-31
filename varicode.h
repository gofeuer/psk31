#ifndef VARICODE_H
#define VARICODE_H

#define VARICODE_LETTER_GAP 2 // 00

static inline unsigned char get_bit_count(unsigned short val) {
    unsigned char count = 1;
    if (val > 0xFF) { count += 8; val >>= 8; }
    if (val > 0x0F) { count += 4; val >>= 4; }
    if (val > 0x03) { count += 2; val >>= 2; }
    if (val > 0x01) { count += 1;            }
    return count;
}

#ifndef PSK31_VARICODE_TABLE
extern const unsigned short varicode_table[128];
#else
const unsigned short varicode_table[128] = { 
    0x2AB,   // 1010101011 NUL (Null character)
    0x2DB,   // 1011011011 SOH (Start of Heading)
    0x2ED,   // 1011101101 STX (Start of Text)
    0x377,   // 1101110111 ETX (End of Text)
    0x2EB,   // 1011101011 EOT (End of Transmission)
    0x35F,   // 1101011111 ENQ (Enquiry)
    0x2EF,   // 1011101111 ACK (Acknowledge)
    0x2FD,   // 1011111101 BEL (Bell / Alert)
    0x2FF,   // 1011111111 BS  (Backspace)
    0xEF,    //   11101111 HT  (Horizontal Tab)
    0x1D,    //      11101 LF  (Line Feed / New Line)
    0x36F,   // 1101101111 VT  (Vertical Tab)
    0x2DD,   // 1011011101 FF  (Form Feed / Page Break)
    0x1F,    //      11111 CR  (Carriage Return)
    0x375,   // 1101110101 SO  (Shift Out)
    0x3AB,   // 1110101011 SI  (Shift In)
    0x2F7,   // 1011110111 DLE (Data Link Escape)
    0x2F5,   // 1011110101 DC1 (Device Control 1 / XON)
    0x3AD,   // 1110101101 DC2 (Device Control 2)
    0x3AF,   // 1110101111 DC3 (Device Control 3 / XOFF)
    0x35B,   // 1101011011 DC4 (Device Control 4)
    0x36B,   // 1101101011 NAK (Negative Acknowledge)
    0x36D,   // 1101101101 SYN (Synchronous Idle)
    0x357,   // 1101010111 ETB (End of Transmission Block)
    0x37B,   // 1101111011 CAN (Cancel)
    0x37D,   // 1101111101 EM  (End of Medium)
    0x3D7,   // 1110110111 SUB (Substitute)
    0x355,   // 1101010101 ESC (Escape)
    0x35D,   // 1101011101 FS  (File Separator)
    0x3BB,   // 1110111011 GS  (Group Separator)
    0x2FB,   // 1011111011 RS  (Record Separator)
    0x37F,   // 1101111111 US  (Unit Separator)

    0x1,     //          1 ' '
    0x1FF,   //  111111111 '!'
    0x15F,   //  101011111 '"'
    0x1F5,   //  111110101 '#'
    0x1DB,   //  111011011 '$'
    0x2D5,   // 1011010101 '%'
    0x2BB,   // 1010111011 '&'
    0x17F,   //  101111111 '''
    0x0FB,   //   11111011 '('
    0xF7,    //   11110111 ')'
    0x16F,   //  101101111 '*'
    0x1DF,   //  111011111 '+'
    0x75,    //    1110101 ','
    0x35,    //     110101 '-'
    0x57,    //    1010111 '.'
    0x1AF,   //  110101111 '/'
    0xB7,    //   10110111 '0'
    0xBD,    //   10111101 '1'
    0xED,    //   11101101 '2'
    0xFF,    //   11111111 '3'
    0x177,   //  101110111 '4'
    0x15B,   //  101011011 '5'
    0x16B,   //  101101011 '6'
    0x1AD,   //  110101101 '7'
    0x1AB,   //  110101011 '8'
    0x1B7,   //  110110111 '9'
    0xF5,    //   11110101 ':'
    0x1BD,   //  110111101 ';'
    0x1ED,   //  111101101 '<'
    0x55,    //    1010101 '='
    0x1D7,   //  111010111 '>'
    0x2AF,   // 1010101111 '?'

    0x2BD,   // 1010111101 '@'
    0x7D,    //    1111101 'A'
    0xEB,    //   11101011 'B'
    0xAD,    //   10101101 'C'
    0xB5,    //   10110101 'D'
    0x77,    //    1110111 'E'
    0xDB,    //   11011011 'F'
    0xFD,    //   11111101 'G'
    0x155,   //  101010101 'H'
    0x7F,    //    1111111 'I'
    0x1FD,   //  111111101 'J'
    0x17D,   //  101111101 'K'
    0xD7,    //   11010111 'L'
    0xBB,    //   10111011 'M'
    0xDD,    //   11011101 'N'
    0xAB,    //   10101011 'O'
    0xD5,    //   11010101 'P'
    0x1DD,   //  111011101 'Q'
    0xAF,    //   10101111 'R'
    0x6F,    //    1101111 'S'
    0x6D,    //    1101101 'T'
    0x157,   //  101010111 'U'
    0x1B5,   //  110110101 'V'
    0x15D,   //  101011101 'W'
    0x175,   //  101110101 'X'
    0x17B,   //  101111011 'Y'
    0x2AD,   // 1010101101 'Z'
    0x1F7,   //  111110111 '['
    0x1EF,   //  111101111 '\'
    0x1FB,   //  111111011 ']'
    0x2BF,   // 1010111111 '^'
    0x16D,   //  101101101 '_'

    0x2DF,   // 1011011111 '`'
    0xB,     //       1011 'a'
    0x5F,    //    1011111 'b'
    0x2F,    //     101111 'c'
    0x2D,    //     101101 'd'
    0x3,     //         11 'e'
    0x3D,    //     111101 'f'
    0x5B,    //    1011011 'g'
    0x2B,    //     101011 'h'
    0xD,     //       1101 'i'
    0x1EB,   //  111101011 'j'
    0xBF,    //   10111111 'k'
    0x1B,    //      11011 'l'
    0x3B,    //     111011 'm'
    0xF,     //       1111 'n'
    0x7,     //        111 'o'
    0x3F,    //     111111 'p'
    0x1BF,   //  110111111 'q'
    0x15,    //      10101 'r'
    0x17,    //      10111 's'
    0x5,     //        101 't'
    0x37,    //     110111 'u'
    0x7B,    //    1111011 'v'
    0x6B,    //    1101011 'w'
    0xDF,    //   11011111 'x'
    0x5D,    //    1011101 'y'
    0x1D5,   //  111010101 'z'
    0x2B7,   // 1010110111 '{'
    0x1BB,   //  110111011 '|'
    0x2B5,   // 1010110101 '}'
    0x2D7,   // 1011010111 '~'
    0x3B5,   // 1110110101 DEL (Delete)
};
#endif

#endif // VARICODE_H
