#ifndef _HDLC_H
#define _HDLC_H

#include "usart.h"
#include <stdbool.h>
#define HDLC_FLAG 0x7E
#define HDLC_ESC 0x7D
#define HDLC_ESC_MASK 0x20
#define MAX_HDLC_LEN 128

void hdlc_encode_and_send(uint8_t *input, size_t length);
void hdlc_encode(uint8_t *input, size_t length, uint8_t *output, size_t* output_length);
void hdlc_decode_and_save(uint8_t *input, size_t length);
void hdlc_decode(uint8_t *input, size_t length, uint8_t *output, size_t *output_length);

#endif
