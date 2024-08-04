#include "hdlc.h"
#include "stm32f1xx.h"
#include "usart.h"

#define CRC_POLYNOMIAL 0x8408

uint8_t encode[MAX_HDLC_LEN];
uint16_t calculate_crc(uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];

        for (size_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ CRC_POLYNOMIAL;
            } else {
                crc = (crc >> 1);
            }
        }
    }

    return crc;
}

void hdlc_encode_and_send(uint8_t *input, size_t length) {
    size_t encode_index = 0;
    hdlc_encode(input, length, encode, &encode_index);
    hdlc_decode_and_save(encode, encode_index);
    Serial_SendArray(encode, encode_index);
}

void hdlc_encode(uint8_t *input, size_t length, uint8_t *output,
                 size_t *output_length) {
    size_t encode_index = 1;
    uint16_t crc = calculate_crc(input, length);
    output[0] = HDLC_FLAG;
    *(input+(length++)) = (uint8_t)(crc & 0xFF);        //LSB
    *(input+(length++)) = (uint8_t)((crc >> 8) & 0xFF); //MSB
    for (size_t i = 0; i < length; i++) {
        if (input[i] == HDLC_FLAG || input[i] == HDLC_ESC) {
            output[encode_index++] = HDLC_ESC;
            output[encode_index++] = input[i] ^ HDLC_ESC_MASK;
        } else {
            output[encode_index++] = input[i];
        }
    }
    output[encode_index++] = HDLC_FLAG;
    *output_length = encode_index;
}

uint8_t decode[MAX_HDLC_LEN];
void hdlc_decode_and_save(uint8_t *input, size_t length) {
    size_t decode_index = 0;
    hdlc_decode(input, length, decode, &decode_index);
}

void hdlc_decode(uint8_t *input, size_t length, uint8_t *output,
                 size_t *output_length) {
    size_t decode_index = 0;
    bool in_frame = false;
    bool crc_match = false;

    for (size_t i = 0; i < length; i++) {
        if (!in_frame) {
            if (input[i] == HDLC_FLAG) {
                in_frame = true;
            }
        } else {
            if (input[i] == HDLC_FLAG) {
                break;
            } else if (input[i] == HDLC_ESC) {
                i++;
                output[decode_index++] = input[i] ^ HDLC_ESC_MASK;
            } else {
                output[decode_index++] = input[i];
            }
        }
    }

    size_t data_fram_len = decode_index - 2;
    uint8_t crc_fram[2];
    crc_fram[0] = output[decode_index - 2];
    crc_fram[1] = output[decode_index - 1];

    uint16_t crc_calculated = calculate_crc(output, data_fram_len);
    uint16_t crc_from_frame = (crc_fram[1] << 8) | crc_fram[0];

    if (crc_calculated == crc_from_frame) {
        crc_match = true;
    }

    if (!crc_match) {
        *output_length = 0;
    } else {
        *output_length = data_fram_len;
    }
}


void test_hdlc() {
    uint8_t data[] = {0x7E, 0x61, 0x7D, 0x5E, 0x7E};
    // hdlc_encode(data, 5);
}