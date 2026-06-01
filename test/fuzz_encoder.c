#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../psk31.h"
#include "test.h"

#define MAX_INPUT_SIZE 256
#define MAX_BUFFER_SIZE (MAX_INPUT_SIZE * 20)  // Varicode max ~10 bits per char + 2 bit gap
#define NUM_ITERATIONS 100000

// Fuzzer state
static struct {
    char decoded[MAX_INPUT_SIZE + 1];
    int decoded_count;
    int test_passed;
    int test_failed;
    int test_crashed;
} fuzzer_state;

// Callback for decoder
static void fuzz_callback(char ascii) {
    if (fuzzer_state.decoded_count < MAX_INPUT_SIZE) {
        fuzzer_state.decoded[fuzzer_state.decoded_count++] = ascii;
    }
    fuzzer_state.decoded[fuzzer_state.decoded_count] = '\0';
}

// Generate random ASCII input (printable ASCII only, to avoid control character issues)
static void generate_random_input(char *input, int *length) {
    int len;
    int i;
    int rand_byte;
    
    len = (rand() % (MAX_INPUT_SIZE - 1)) + 1;  // 1 to MAX_INPUT_SIZE chars
    
    for (i = 0; i < len; i++) {
        // Use printable ASCII only (space to ~)
        rand_byte = 32 + (rand() % 95);
        input[i] = (char)rand_byte;
    }
    input[len] = '\0';
    *length = len;
}

// Test encode/decode round-trip
static int test_round_trip(const char *input, int input_len, int buffer_size) {
    unsigned short *encode_words;
    unsigned char *decode_stream;
    int stream_length;
    int decode_stream_length;
    int word_count;
    int i;
    int success;
    int expected_output_len;
    unsigned char in_byte;
    unsigned char out_byte;
    int out_idx;
    
    word_count = ((buffer_size + 1) / 2) + 2;
    encode_words = (unsigned short *)malloc(sizeof(unsigned short) * word_count);
    if (!encode_words) {
        fprintf(stderr, "ERROR: Failed to allocate encode buffer\n");
        return 0;
    }
    memset(encode_words, 0, sizeof(unsigned short) * word_count);
    
    // Initialize encoder with leading/trailing zero words around the encoded stream.
    encoder_start((unsigned char *)&encode_words[1]);
    for (i = 0; i < input_len; i++) {
        encoder_push(input[i]);
    }
    stream_length = encoder_done();
    
    // Check if stream fits in buffer
    if (stream_length > buffer_size) {
        fprintf(stderr, "ERROR: Stream length %d exceeds buffer size %d\n", 
                stream_length, buffer_size);
        free(encode_words);
        return 0;
    }
    
    // Decode the output
    fuzzer_state.decoded_count = 0;
    decoder_init(fuzz_callback);
    
    decode_stream = (unsigned char *)encode_words;
    decode_stream_length = stream_length + 4;
    for (i = 0; i < decode_stream_length; i++) {
        decoder_push(decode_stream[i]);
    }
    
    // Verify correctness: decoded should match valid ASCII input only
    // Invalid ASCII (>127) is silently ignored by encoder
    success = 1;
    expected_output_len = 0;
    out_idx = 0;
    
    // Count expected output length (only valid ASCII)
    for (i = 0; i < input_len; i++) {
        if ((unsigned char)input[i] <= 127) {
            expected_output_len++;
        }
    }
    
    if (fuzzer_state.decoded_count != expected_output_len) {
        fprintf(stderr, "ERROR: Decoded count mismatch. Expected %d, got %d\n", 
                expected_output_len, fuzzer_state.decoded_count);
        
        // Find which character is missing
        if (expected_output_len > fuzzer_state.decoded_count) {
            fprintf(stderr, "  Missing character(s) in output\n");
            for (i = 0; i < input_len && out_idx < fuzzer_state.decoded_count; i++) {
                in_byte = (unsigned char)input[i];
                if (in_byte <= 127) {
                    out_byte = (unsigned char)fuzzer_state.decoded[out_idx];
                    if (in_byte != out_byte) {
                        fprintf(stderr, "  Mismatch at position %d: expected 0x%02X, got 0x%02X\n", 
                                out_idx, in_byte, out_byte);
                        break;
                    }
                    out_idx++;
                }
            }
        }
        success = 0;
    } else {
        // Compare byte by byte (only valid ASCII from input)
        out_idx = 0;
        for (i = 0; i < input_len; i++) {
            in_byte = (unsigned char)input[i];
            
            // Skip invalid ASCII - they're not encoded
            if (in_byte > 127) {
                continue;
            }
            
            out_byte = (unsigned char)fuzzer_state.decoded[out_idx];
            
            // Valid ASCII should round-trip perfectly
            if (in_byte != out_byte) {
                fprintf(stderr, "ERROR: Round-trip mismatch at position %d\n", i);
                fprintf(stderr, "  Expected: 0x%02X ('%c')\n", in_byte, 
                        (in_byte >= 32 && in_byte < 127) ? (char)in_byte : '?');
                fprintf(stderr, "  Got:      0x%02X ('%c')\n", out_byte,
                        (out_byte >= 32 && out_byte < 127) ? (char)out_byte : '?');
                success = 0;
                break;
            }
            out_idx++;
        }
    }
    
    free(encode_words);
    return success;
}

// Run a single fuzz test
static int run_fuzz_test(void) {
    char input[MAX_INPUT_SIZE + 1];
    int input_len;
    int buffer_size;
    
    // Generate random input
    generate_random_input(input, &input_len);
    
    // Use a safe buffer size (larger than worst case)
    buffer_size = MAX_BUFFER_SIZE;
    
    // Test round-trip correctness
    if (!test_round_trip(input, input_len, buffer_size)) {
        return 0;
    }
    
    return 1;
}

// Print coverage statistics
static void print_coverage_stats(int iterations) {
    printf("\n=== Coverage Statistics ===\n");
    printf("Total iterations: %d\n", iterations);
    printf("Passed: %d (%.1f%%)\n", fuzzer_state.test_passed, 
           (float)fuzzer_state.test_passed / iterations * 100);
    printf("Failed: %d\n", fuzzer_state.test_failed);
    printf("Crashed: %d\n", fuzzer_state.test_crashed);
    
    if (fuzzer_state.test_failed == 0 && fuzzer_state.test_crashed == 0) {
        printf("\n✓ All tests passed!\n");
    } else {
        printf("\n✗ Issues detected. See details above.\n");
    }
}

int main(int argc, char *argv[]) {
    int num_iterations;
    int i;
    
    srand((unsigned int)time(NULL));
    
    num_iterations = NUM_ITERATIONS;
    
    // Allow command-line override
    if (argc > 1) {
        num_iterations = atoi(argv[1]);
        if (num_iterations <= 0) {
            fprintf(stderr, "Invalid iteration count: %s\n", argv[1]);
            return 1;
        }
    }
    
    printf("PSK31 Encoder Fuzzer\n");
    printf("Running %d fuzz iterations...\n\n", num_iterations);
    
    for (i = 0; i < num_iterations; i++) {
        if (run_fuzz_test()) {
            fuzzer_state.test_passed++;
        } else {
            fuzzer_state.test_failed++;
        }
        
        // Print progress every 10k iterations
        if ((i + 1) % 10000 == 0) {
            printf("Progress: %d/%d iterations\n", i + 1, num_iterations);
        }
    }
    
    print_coverage_stats(num_iterations);
    
    // Exit with status based on failures
    return (fuzzer_state.test_failed > 0 || fuzzer_state.test_crashed > 0) ? 1 : 0;
}
