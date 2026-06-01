#ifndef PSK31_H
#define PSK31_H

// ---------------------------------------------------------------------
// Encoder
// ---------------------------------------------------------------------

/**
 * Initializes the encoder with a buffer.
 * @param buffer The buffer to store the encoded text.
 */
void encoder_start(unsigned char *buffer);

/**
 * Pushes a character to the encoder.
 * @param ascii The ASCII character to push.
 */
void encoder_push(char ascii);

/**
 * Finalizes the encoding and returns the resulting stream length.
 * @return The length of the stream in bytes.
 */
int encoder_done(void);

// ---------------------------------------------------------------------
// Decoder
// ---------------------------------------------------------------------

/**
 * Callback function type for decoded ASCII characters.
 * @param ascii The decoded ASCII character.
 */
typedef void(*ascii_callback)(char ascii);

/**
 * Initializes the decoder with a callback function.
 * @param callback The callback function to invoke when characters are decoded.
 */
void decoder_init(ascii_callback callback);

/**
 * Pushes a byte to the decoder for processing.
 * @param byte The byte to decode.
 */
void decoder_push(unsigned char byte);

#endif // PSK31_H
