import numpy as np
HDLC_FLAG = 0x7E
HDLC_ESC = 0x7D
HDLC_ESC_MASK = 0x20
CRC_POLYNOMIAL = 0x8408

def calculate_crc(data):
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ CRC_POLYNOMIAL
            else:
                crc = crc >> 1
    return crc

def hdlc_encode(input):
    encoded = bytearray([HDLC_FLAG])
    crc = calculate_crc(input)
    input_array = bytearray([*input, crc & 0xFF, (crc >> 8) & 0xFF])
    for byte in input_array:
        if byte == HDLC_FLAG or byte == HDLC_ESC:
            encoded.extend([HDLC_ESC, byte ^ HDLC_ESC_MASK])
        else:
            encoded.append(byte)
    encoded.extend([HDLC_FLAG])
    return encoded

def hdlc_decode(input):
    decoded = bytearray()
    in_frame = False
    crc_match = False
    inter = iter(range(len(input)))
    for i in inter:
        byte = input[i]
        if not in_frame:
            if byte == HDLC_FLAG:
                in_frame = True
        else:
            if byte == HDLC_FLAG:
                break
            elif  byte == HDLC_ESC:
                decoded.append(input[i+1] ^ HDLC_ESC_MASK)
                next(inter)
            else:
                decoded.append(byte)
    data_fram = decoded[:-2]
    crc_fram = decoded[-2:]
    crc_calculated = calculate_crc(data_fram)
    crc_from_frame = (crc_fram[1] << 8) |crc_fram[0] 
    if crc_calculated == crc_from_frame:
        crc_match = True
    if not crc_match:
        return None
    else:
        return data_fram

if __name__ == "__main__":
    # Example usage
    data = bytearray([0x7E, 0x61, 0x7D, 0x5E, 0x7E])
    encoded = hdlc_encode(data)
    print(['0x{:02X}'.format(byte) for byte in encoded])
    decoded = hdlc_decode(encoded)
    print(decoded == data)