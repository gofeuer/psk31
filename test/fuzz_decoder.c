#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../psk31.h"

#define DEFAULT_RANDOM_STREAM_CASES 10000u
#define DEFAULT_ROUNDTRIP_CASES 5000u
#define DEFAULT_MIXED_CORRUPTION_CASES 5000u
#define MAX_RANDOM_STREAM_BYTES 512u
#define MAX_RANDOM_TEXT_BYTES 128u
#define MAX_OVERLONG_VARICODE_BITS 64u
#define ROUNDTRIP_WORDS 512u

// Fuzzer state
static struct {
    char decoded[4096];
    unsigned int decoded_count;
    unsigned int decoded_overflow;
    unsigned int test_passed;
    unsigned int test_failed;
} fuzz_state;

static unsigned long rng_state = 0x12345678ul;

static unsigned int next_random(void) {
    rng_state = (rng_state * 1103515245ul) + 12345ul;
    return (unsigned int)(rng_state >> 16);
}

static void capture_ascii(char ascii) {
    if (fuzz_state.decoded_count < sizeof(fuzz_state.decoded)) {
        fuzz_state.decoded[fuzz_state.decoded_count++] = ascii;
    } else {
        fuzz_state.decoded_overflow = 1u;
    }
}

static void reset_capture(void) {
    fuzz_state.decoded_count = 0u;
    fuzz_state.decoded_overflow = 0u;
    memset(fuzz_state.decoded, 0, sizeof(fuzz_state.decoded));
}

static void push_bits(const unsigned char *bits, unsigned int bit_count) {
    unsigned char byte;
    unsigned int i;

    byte = 0u;
    for (i = 0u; i < bit_count; i++) {
        byte <<= 1;
        byte |= bits[i] ? 1u : 0u;

        if ((i & 7u) == 7u) {
            decoder_push(byte);
            byte = 0u;
        }
    }

    if ((bit_count & 7u) != 0u) {
        byte <<= 8u - (bit_count & 7u);
        decoder_push(byte);
    }
}

static int fuzz_random_streams(unsigned int cases) {
    unsigned char stream[MAX_RANDOM_STREAM_BYTES];
    unsigned int case_index;
    unsigned int length;
    unsigned int i;

    for (case_index = 0u; case_index < cases; case_index++) {
        reset_capture();
        decoder_init(capture_ascii);

        length = next_random() % (MAX_RANDOM_STREAM_BYTES + 1u);
        for (i = 0u; i < length; i++) {
            stream[i] = (unsigned char)next_random();
            decoder_push(stream[i]);
        }

        if (fuzz_state.decoded_overflow) {
            fprintf(stderr, "ERROR: random stream produced too much output at case %u\n", case_index);
            return 1;
        }
    }

    return 0;
}

// Encode a single ASCII character to bits and append to bit buffer
static void encode_char_to_bits(char ascii, unsigned char *bits, unsigned int *bit_pos) {
    if (ascii == 'a') {
        // 'a' = 0xB (1011) - 4 bits + 2-bit gap (00)
        bits[(*bit_pos)++] = 1u;
        bits[(*bit_pos)++] = 0u;
        bits[(*bit_pos)++] = 1u;
        bits[(*bit_pos)++] = 1u;
        bits[(*bit_pos)++] = 0u;
        bits[(*bit_pos)++] = 0u;
    } else if (ascii == 'e') {
        // 'e' = 0x3 (11) - 2 bits + 2-bit gap (00)
        bits[(*bit_pos)++] = 1u;
        bits[(*bit_pos)++] = 1u;
        bits[(*bit_pos)++] = 0u;
        bits[(*bit_pos)++] = 0u;
    } else if (ascii == ' ') {
        // space = 0x1 (1) - 1 bit + 2-bit gap (00)
        bits[(*bit_pos)++] = 1u;
        bits[(*bit_pos)++] = 0u;
        bits[(*bit_pos)++] = 0u;
    } else if (ascii == 't') {
        // 't' = 0x5 (101) - 3 bits + 2-bit gap (00)
        bits[(*bit_pos)++] = 1u;
        bits[(*bit_pos)++] = 0u;
        bits[(*bit_pos)++] = 1u;
        bits[(*bit_pos)++] = 0u;
        bits[(*bit_pos)++] = 0u;
    }
}

static int fuzz_mixed_corruption(unsigned int cases) {
    unsigned char bits[1024];
    unsigned int case_index;
    unsigned int bit_pos;
    unsigned int corruption_bit_count;
    unsigned int i;
    unsigned int expected_chars;
    unsigned int pre_chars;
    unsigned int post_chars;

    for (case_index = 0u; case_index < cases; case_index++) {
        reset_capture();
        decoder_init(capture_ascii);

        bit_pos = 0u;
        
        // Encode 1-3 valid characters at the start
        pre_chars = 1u + (next_random() % 3u);
        for (i = 0u; i < pre_chars; i++) {
            encode_char_to_bits('a', bits, &bit_pos);
        }
        expected_chars = pre_chars;
        
        // Add a corrupted overlong varicode (11-40 bits with random pattern)
        corruption_bit_count = 11u + (next_random() % 30u);
        bits[bit_pos++] = 1u;
        for (i = 1u; i < corruption_bit_count - 1u; i++) {
            unsigned char rand_bit = (unsigned char)(next_random() & 1u);
            // Avoid consecutive zeros to prevent premature termination
            if ((i > 0u) && (bits[bit_pos - 1u] == 0u) && (rand_bit == 0u)) {
                rand_bit = 1u;
            }
            bits[bit_pos++] = rand_bit;
        }
        bits[bit_pos++] = 1u;
        // Add 2 zero bits to terminate the corrupted varicode
        bits[bit_pos++] = 0u;
        bits[bit_pos++] = 0u;
        
        // Encode 1-3 valid characters after the corruption
        post_chars = 1u + (next_random() % 3u);
        for (i = 0u; i < post_chars; i++) {
            encode_char_to_bits('e', bits, &bit_pos);
        }
        expected_chars += post_chars;
        
        // Push all bits to decoder
        push_bits(bits, bit_pos);
        
        // The decoder should:
        // 1. Decode pre-corruption characters ('a' x pre_chars)
        // 2. Replace overlong varicode with space (or handle it gracefully)
        // 3. Decode post-corruption characters ('e' x post_chars)
        
        if (fuzz_state.decoded_overflow) {
            fprintf(stderr, 
                    "ERROR: mixed corruption produced too much output at case %u\n", 
                    case_index);
            return 1;
        }
        
        if (fuzz_state.decoded_count < expected_chars) {
            fprintf(stderr, 
                    "ERROR: mixed corruption lost characters at case %u: expected at least %u, got %u\n",
                    case_index,
                    expected_chars,
                    fuzz_state.decoded_count);
            fprintf(stderr, "  Pre-chars: %u, Corruption bits: %u, Post-chars: %u\n",
                    pre_chars, corruption_bit_count, post_chars);
            return 1;
        }
    }

    return 0;
}

static int fuzz_roundtrips(unsigned int cases) {
    unsigned short words[ROUNDTRIP_WORDS];
    char expected[MAX_RANDOM_TEXT_BYTES];
    unsigned int case_index;
    unsigned int text_length;
    unsigned int i;
    int encoded_length;
    unsigned char *stream;
    int stream_length;

    for (case_index = 0u; case_index < cases; case_index++) {
        reset_capture();
        decoder_init(capture_ascii);

        memset(words, 0, sizeof(words));
        text_length = next_random() % (MAX_RANDOM_TEXT_BYTES + 1u);

        encoder_start((unsigned char *)&words[1]);
        for (i = 0u; i < text_length; i++) {
            expected[i] = (char)(next_random() & 0x7Fu);
            encoder_push(expected[i]);
        }
        encoded_length = encoder_done();

        stream = (unsigned char *)words;
        stream_length = encoded_length + 4;
        for (i = 0u; i < (unsigned int)stream_length; i++) {
            decoder_push(stream[i]);
        }

        if (fuzz_state.decoded_overflow || (fuzz_state.decoded_count != text_length) ||
            (memcmp(fuzz_state.decoded, expected, text_length) != 0)) {
            fprintf(stderr,
                    "ERROR: roundtrip mismatch at case %u: expected %u bytes, decoded %u bytes\n",
                    case_index,
                    text_length,
                    fuzz_state.decoded_count);
            return 1;
        }
    }

    return 0;
}

static unsigned int parse_count(const char *argument, unsigned int fallback) {
    int parsed;

    if (argument == NULL) {
        return fallback;
    }

    parsed = atoi(argument);
    if (parsed <= 0) {
        return fallback;
    }

    return (unsigned int)parsed;
}

int main(int argc, char *argv[]) {
    unsigned int random_stream_cases;
    unsigned int roundtrip_cases;
    unsigned int mixed_corruption_cases;

    random_stream_cases = parse_count((argc > 1) ? argv[1] : NULL, DEFAULT_RANDOM_STREAM_CASES);
    roundtrip_cases = parse_count((argc > 2) ? argv[2] : NULL, DEFAULT_ROUNDTRIP_CASES);
    mixed_corruption_cases = parse_count((argc > 3) ? argv[3] : NULL, DEFAULT_MIXED_CORRUPTION_CASES);

    printf("PSK31 Decoder Fuzzer\n");
    printf("Running %u random streams, %u round trips, %u mixed corruption cases...\n\n",
           random_stream_cases, roundtrip_cases, mixed_corruption_cases);

    if (fuzz_random_streams(random_stream_cases)) {
        fprintf(stderr, "FAILED: random stream fuzzing\n");
        return 1;
    }
    printf("✓ Random stream fuzzing passed\n");

    if (fuzz_roundtrips(roundtrip_cases)) {
        fprintf(stderr, "FAILED: roundtrip fuzzing\n");
        return 1;
    }
    printf("✓ Roundtrip fuzzing passed\n");

    if (fuzz_mixed_corruption(mixed_corruption_cases)) {
        fprintf(stderr, "FAILED: mixed corruption fuzzing\n");
        return 1;
    }
    printf("✓ Mixed corruption fuzzing passed\n");

    printf("\n=== Fuzzing Results ===\n");
    printf("Random streams:      %u cases\n", random_stream_cases);
    printf("Roundtrips:          %u cases\n", roundtrip_cases);
    printf("Mixed corruption:    %u cases\n", mixed_corruption_cases);
    printf("Total cases:         %u\n", random_stream_cases + roundtrip_cases + mixed_corruption_cases);
    printf("\n✓ All decoder fuzz tests passed!\n");

    return 0;
}
