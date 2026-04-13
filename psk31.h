#ifndef PSK31_H
#define PSK31_H
#include <stdint.h>

    void encoder_start(uint16_t *buffer);
    void encoder_push(const uint8_t ascii_char);
uint16_t encoder_done(void);

#endif // PSK31_H
