/*
 * util.c
 *
 *  Created on: Aug 27, 2023
 *      Author: Sameen
 */


#include "util.h"


uint16_t
crc16_compute(uint8_t* data, size_t size)
{
	// black magic; no clue how it works but it works
	uint16_t crc = 0xffff;
	for (int i = 0; i < size; i++)
	{
		crc ^= data[i];
		for (int j = 0; j < 8; j++)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ 0xa001;
			else
				crc >>= 1;
		}
	}
	return crc;
}



static char
hex_to_char (uint8_t h)
{
    char c;
    if (h >= 0 && h <= 9)
    {
        c = h + '0';
    }
    else if (h >= 10 && h <= 15)
    {
        c = h - 10 + 'A';
    }
    else
    {
        c = 0;
    }
    return c;
}



static uint8_t
char_to_hex (char c)
{
    uint8_t r;
    if (c >= '0' && c <= '9')
    {
        r = c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        r = (c - 'A') + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        r = (c - 'a') + 10;
    }
    else
    {
        r = 0;
    }
    return r;
}



/* Modbus ASCII helper functions */

/**
 * @brief Converts Modbus RTU frame into Modbus ASCII frame.
 *
 * @param ascii_msg 	ASCII frame output
 * @param rtu_msg 		RTU frame input
 * @param rtu_msg_size 	RTU frame length
 */
size_t
mbascii_from_rtu (uint8_t *ascii_msg, uint8_t *rtu_msg, size_t rtu_msg_size)
{
	uint8_t lrc = 0;

	// check if message size is invalid
    if (rtu_msg_size > 256 || rtu_msg_size < 4)
    {
        return 0;
    }

    size_t ascii_msg_size = (rtu_msg_size * 2) - 1;
	
	ascii_msg[0] = ':';
    
    for (size_t i = 0; i < rtu_msg_size - 2; i++)
    {
        uint8_t msg_byte = rtu_msg[i];
        ascii_msg[2 * i + 1] = hex_to_char((msg_byte >> 4) & 0xF);
        ascii_msg[2 * i + 2] = hex_to_char(msg_byte & 0xF);
        lrc += rtu_msg[i];
    }

    lrc = -lrc;

    ascii_msg[ascii_msg_size - 2] = hex_to_char((lrc >> 4) & 0xF);
    ascii_msg[ascii_msg_size - 1] = hex_to_char(lrc & 0xF);
    ascii_msg[ascii_msg_size] = '\0';

    return ascii_msg_size;
}



size_t
mbascii_to_rtu (uint8_t *ascii_msg, uint8_t *rtu_msg, size_t ascii_msg_size)
{
    if (ascii_msg[0] != ':')
    {
        return 0;
    }

    size_t rtu_msg_size = ascii_msg_size / 2 + 1;

    for (size_t i = 0; i < rtu_msg_size - 2; i++)
    {
        uint8_t msg_byte = 0;

        msg_byte |= (char_to_hex(ascii_msg[2 * i + 1]) << 4);
        msg_byte |= char_to_hex(ascii_msg[2 * i + 2]);

        rtu_msg[i] = msg_byte;
    }

    uint16_t crc = crc16_compute(rtu_msg, rtu_msg_size - 2);

    rtu_msg[rtu_msg_size - 2] = crc & 0xFF;
    rtu_msg[rtu_msg_size - 1] = (crc >> 8) & 0xFF;

    return rtu_msg_size;
}
