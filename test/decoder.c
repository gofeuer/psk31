#include <stdio.h>
#include <string.h>
#include "../psk31.h"
#include "../varicode.h"
#include "test.h"

// Test state
static struct {
    char decoded[256];
    int decoded_count;
} test_state;

// Callback for decoder tests
static void test_callback(char ascii) {
    if (test_state.decoded_count < 255) {
        test_state.decoded[test_state.decoded_count++] = ascii;
    }
    test_state.decoded[test_state.decoded_count] = '\0';
}

// Test: Single character decoding using encoder
static void test_single_char_space(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push(' ');
    int stream_length = encoder_done();
    
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 1, "Space character should be decoded");
    ASSERT(test_state.decoded[0] == ' ', "Decoded character should be space");
}

// Test: Character 'e' (most common in English)
static void test_single_char_e(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('e');
    int stream_length = encoder_done();
    
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 1, "Character 'e' should be decoded");
    ASSERT(test_state.decoded[0] == 'e', "Decoded character should be 'e'");
}

// Test: Character 'a' (4 bits)
static void test_single_char_a(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('a');
    int stream_length = encoder_done();
    
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 1, "Character 'a' should be decoded");
    ASSERT(test_state.decoded[0] == 'a', "Decoded character should be 'a'");
}

// Test: Multiple characters in sequence
static void test_multiple_chars(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('h');
    encoder_push('i');
    int stream_length = encoder_done();
    
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 2, "Two characters should be decoded");
    ASSERT(test_state.decoded[0] == 'h', "First character should be 'h'");
    ASSERT(test_state.decoded[1] == 'i', "Second character should be 'i'");
}

// Test: String "hello"
static void test_string_hello(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    // We need to push the encoded bits for "hello"
    // For simplicity, we'll use the encoder to get the exact bit sequence
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('h');
    encoder_push('e');
    encoder_push('l');
    encoder_push('l');
    encoder_push('o');
    int stream_length = encoder_done();
    
    // Now decode it
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 5, "Five characters should be decoded");
    ASSERT(strcmp(test_state.decoded, "hello") == 0, "Decoded string should be 'hello'");
}

// Test: Digits
static void test_digits(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    // Encode "0123"
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('0');
    encoder_push('1');
    encoder_push('2');
    encoder_push('3');
    int stream_length = encoder_done();
    
    // Decode
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 4, "Four digits should be decoded");
    ASSERT(strcmp(test_state.decoded, "0123") == 0, "Decoded digits should be '0123'");
}

// Test: Special characters
static void test_special_chars(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    // Encode ".,!"
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('.');
    encoder_push(',');
    encoder_push('!');
    int stream_length = encoder_done();
    
    // Decode
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 3, "Three special characters should be decoded");
    ASSERT(strcmp(test_state.decoded, ".,!") == 0, "Decoded special chars should be '.,!'");
}

// Test: Line feed and carriage return
static void test_line_feed_cr(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    // Encode text with LF and CR
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('a');
    encoder_push('\n');
    encoder_push('b');
    encoder_push('\r');
    encoder_push('c');
    int stream_length = encoder_done();
    
    // Decode
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 5, "Five characters including LF/CR should be decoded");
    ASSERT(test_state.decoded[0] == 'a', "First character should be 'a'");
    ASSERT(test_state.decoded[1] == '\n', "Second character should be LF");
    ASSERT(test_state.decoded[2] == 'b', "Third character should be 'b'");
    ASSERT(test_state.decoded[3] == '\r', "Fourth character should be CR");
    ASSERT(test_state.decoded[4] == 'c', "Fifth character should be 'c'");
}

// Test: Mixed case letters
static void test_mixed_case(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    // Encode "AaBbCc"
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('A');
    encoder_push('a');
    encoder_push('B');
    encoder_push('b');
    encoder_push('C');
    encoder_push('c');
    int stream_length = encoder_done();
    
    // Decode
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 6, "Six characters should be decoded");
    ASSERT(strcmp(test_state.decoded, "AaBbCc") == 0, "Decoded mixed case should match");
}

// Test: Initialization doesn't affect previous state
static void test_reinit(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    // Encode and decode 'a'
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('a');
    int stream_length = encoder_done();
    
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    int first_count = test_state.decoded_count;
    
    // Re-initialize decoder
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    // Encode and decode 'b'
    encoder_start(encoded_buffer);
    encoder_push('b');
    stream_length = encoder_done();
    
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(first_count == 1, "First decode should produce one character");
    ASSERT(test_state.decoded_count == 1, "Second decode should produce one character");
    ASSERT(test_state.decoded[0] == 'b', "Second character should be 'b' after re-init");
}

// Test: Long string
static void test_long_string(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    const char *test_string = "The quick brown fox jumps over the lazy dog";
    
    unsigned char encoded_buffer[512];
    encoder_start(encoded_buffer);
    for (int i = 0; test_string[i] != '\0'; i++) {
        encoder_push(test_string[i]);
    }
    int stream_length = encoder_done();
    
    // Decode
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT((size_t)test_state.decoded_count == strlen(test_string), "All characters should be decoded");
    ASSERT(strcmp(test_state.decoded, test_string) == 0, "Decoded string should match original");
}

// Test: Tab character
static void test_tab_character(void) {
    test_state.decoded_count = 0;
    decoder_init(test_callback);
    
    unsigned char encoded_buffer[256];
    encoder_start(encoded_buffer);
    encoder_push('a');
    encoder_push('\t');
    encoder_push('b');
    int stream_length = encoder_done();
    
    for (int i = 0; i < stream_length; i++) {
        decoder_push(encoded_buffer[i]);
    }
    
    ASSERT(test_state.decoded_count == 3, "Three characters should be decoded");
    ASSERT(test_state.decoded[0] == 'a', "First character should be 'a'");
    ASSERT(test_state.decoded[1] == '\t', "Second character should be tab");
    ASSERT(test_state.decoded[2] == 'b', "Third character should be 'b'");
}

int main(void) {
    printf("PSK31 Decoder Unit Tests\n");
    printf("========================\n\n");
    
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
    
    test_reinit();
    printf("✓ Re-initialization\n");
    
    test_long_string();
    printf("✓ Long string decode\n");
    
    test_tab_character();
    printf("✓ Tab character\n");
    
    printf("\n✓ All tests passed!\n");
    return 0;
}
