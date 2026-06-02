#include "../psk31.h"
#include <stdio.h>
#include <string.h>
#include "test.h"

// Hello World
//     H        e       l        l       o     -        W         o       r        l        d
// 101010101 00 11 00 11011 00 11011 00 111 00 1 00 101011101 00 111 00 10101 00 11011 00 101101

// Test: Encoder initialization
static void test_encoder_init(void) {
    unsigned short buffer[8];
    memset(buffer, 0xFF, sizeof(buffer)); // "Dirty" buffer
    
    encoder_start((unsigned char*)buffer);
    
    ASSERT(buffer[0] == 0, "First buffer element should be initialized to 0");
}

// Test: Single character encoding
static void test_single_char_space(void) {
    unsigned short buffer[8];
    encoder_start((unsigned char*)buffer);
    
    encoder_push(' ');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should produce encoded bytes for space character");
}

// Test: Single character 'e' (shortest encoding)
static void test_single_char_e(void) {
    unsigned short buffer[8];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('e');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should produce encoded bytes for 'e'");
}

// Test: Single character 'a'
static void test_single_char_a(void) {
    unsigned short buffer[8];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('a');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should produce encoded bytes for 'a'");
}

// Test: Multiple characters
static void test_multiple_chars(void) {
    unsigned short buffer[8];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('h');
    encoder_push('i');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should produce encoded bytes for 'hi'");
}

// Test: String "hello"
static void test_string_hello(void) {
    unsigned short buffer[16];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('h');
    encoder_push('e');
    encoder_push('l');
    encoder_push('l');
    encoder_push('o');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should produce encoded bytes for 'hello'");
    ASSERT(bytes <= 32, "Should fit in buffer");
}

// Test: Digits
static void test_digits(void) {
    unsigned short buffer[16];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('0');
    encoder_push('1');
    encoder_push('2');
    encoder_push('3');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should encode digits");
}

// Test: Special characters
static void test_special_chars(void) {
    unsigned short buffer[16];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('.');
    encoder_push(',');
    encoder_push('!');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should encode special characters");
}

// Test: Line feed and carriage return
static void test_line_feed_cr(void) {
    unsigned short buffer[16];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('a');
    encoder_push('\n');
    encoder_push('b');
    encoder_push('\r');
    encoder_push('c');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should encode text with LF/CR");
}

// Test: Mixed case letters
static void test_mixed_case(void) {
    unsigned short buffer[16];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('A');
    encoder_push('a');
    encoder_push('B');
    encoder_push('b');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should encode mixed case letters");
}

// Test: Long string
static void test_long_string(void) {
    unsigned short buffer[128];
    encoder_start((unsigned char*)buffer);
    
    const char *test_string = "The quick brown fox jumps over the lazy dog";
    for (int i = 0; test_string[i] != '\0'; i++) {
        encoder_push(test_string[i]);
    }
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should encode long string");
    ASSERT(bytes <= 256, "Should fit in buffer");
}

// Test: Tab character
static void test_tab_character(void) {
    unsigned short buffer[16];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('a');
    encoder_push('\t');
    encoder_push('b');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should encode tab character");
}

// Test: Invalid ASCII (>127) should be ignored
static void test_invalid_ascii(void) {
    unsigned short buffer[16];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('a');
    encoder_push((char)200); // Invalid ASCII, should be ignored
    encoder_push('b');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Should handle invalid ASCII gracefully");
}

// Test: Encoder done returns byte count
static void test_encoder_done_returns_bytes(void) {
    unsigned short buffer[8];
    encoder_start((unsigned char*)buffer);
    
    encoder_push('H');
    encoder_push('i');
    int bytes = encoder_done();
    
    ASSERT(bytes > 0, "Encoder done should return positive byte count");
    ASSERT(bytes % 2 == 0, "Byte count should be even (multiples of 2)");
}

int main(void) {
    printf("PSK31 Encoder Unit Tests\n");
    printf("========================\n\n");
    
    test_encoder_init();
    printf("✓ Encoder initialization\n");
    
    test_single_char_space();
    printf("✓ Single character space\n");
    
    test_single_char_e();
    printf("✓ Single character 'e'\n");
    
    test_single_char_a();
    printf("✓ Single character 'a'\n");
    
    test_multiple_chars();
    printf("✓ Multiple characters\n");
    
    test_string_hello();
    printf("✓ String 'hello'\n");
    
    test_digits();
    printf("✓ Digits 0-3\n");
    
    test_special_chars();
    printf("✓ Special characters\n");
    
    test_line_feed_cr();
    printf("✓ Line feed and carriage return\n");
    
    test_mixed_case();
    printf("✓ Mixed case letters\n");
    
    test_long_string();
    printf("✓ Long string encode\n");
    
    test_tab_character();
    printf("✓ Tab character\n");
    
    test_invalid_ascii();
    printf("✓ Invalid ASCII handling\n");
    
    test_encoder_done_returns_bytes();
    printf("✓ Encoder done returns byte count\n");
    
    printf("\n✓ All tests passed!\n");
    return 0;
}
