/*
 * hexador.c
 *
 *  Created on: Mar 6, 2021
 *      Author: nenad
 */

#include "hexador.h"
#include <ctype.h>

static int hex_to_bin(char hex, uint8_t *bin)
{
    static const struct hex_map_to_int
    {
        char from;
        uint8_t to;
    } hex_map[16] = {
        {
            '0',
            0},
        {
            '1',
            1},
        {
            '2',
            2},
        {
            '3',
            3},
        {
            '4',
            4},
        {
            '5',
            5},
        {
            '6',
            6},
        {
            '7',
            7},
        {
            '8',
            8},
        {
            '9',
            9},
        {
            'a',
            10},
        {
            'b',
            11},
        {
            'c',
            12},
        {
            'd',
            13},
        {
            'e',
            14},
        {
            'f',
            15}};

    for (size_t i = 0u; i < 16; i++) {
        if (hex_map[i].from == hex) {
            *bin = hex_map[i].to;
            return 0;
        }
    }
    return -1;
}

static int bin_to_hex(uint8_t bin, char * hex)
{
    static uint8_t bin_map[16] = {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        'a',
        'b',
        'c',
        'd',
        'e',
        'f'
    };
    if (bin > 15) {
        return -1;
    }
    *hex = bin_map[bin];
    return 0;
}

size_t hexador__to_bin(const char *source, uint8_t *destination)
{

    enum hexador__state
    {
        STATE_HIGH,
        STATE_LOW
    } state;
    size_t converted;
    uint8_t hi_half;

    converted = 0u;
    state = STATE_HIGH;
    while (*source) {
        int error;
        const char lower = (char)tolower(*source++);

        switch (state) {
        case STATE_HIGH: {
            error = hex_to_bin(lower, &hi_half);
            if (error) {
                return converted;
            }
            state = STATE_LOW;
            break;
        }
        case STATE_LOW: {
            uint8_t lo_half;
            error = hex_to_bin(lower, &lo_half);
            if (error) {
                return converted;
            }
            *destination++ = (uint8_t)((hi_half << 4) | lo_half);
            converted++;
            state = STATE_HIGH;
            break;
        }
        default:
            return 0u;
        }
    }
    return converted;
}

size_t hexador__to_hex(const uint8_t * source, size_t source_size, char * destination)
{
    size_t converted;

    converted = 0u;
    while (source_size) {
        int error;
        uint8_t msb_half = *source >> 4u;
        uint8_t lsb_half = *source & 0xfu;
        error = bin_to_hex(msb_half, destination);
        if (error) {
            break;
        }
        destination++;
        error = bin_to_hex(lsb_half, destination);
        if (error) {
            break;
        }
        destination++;
        source++;
        converted++;
        source_size--;
    }
    return converted;
}
