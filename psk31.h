#ifndef PSK31_H
#define PSK31_H
#include <stdint.h>

/**
 * Initializes the encoder with a buffer.
 * @param buffer The buffer to use for encoding.
 */
void encoder_start(uint16_t *buffer);

/**
 * Pushes a character to the encoder.
 * @param ascii_char The ASCII character to push.
 */
void encoder_push(const uint8_t ascii_char);

/**
 * Finalizes the encoding and returns the resulting stream.
 * @param stream A pointer to the stream buffer.
 * @return The length of the stream in bytes.
 */
uint16_t encoder_done(uint8_t **stream);

#endif // PSK31_H
