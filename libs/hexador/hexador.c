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

size_t hexador__to_bin(const char *source, uint8_t *destination)
{

    enum hexador__state
    {
        STATE_HIGH,
        STATE_LOW
    } state;
    size_t retval;
    uint8_t hi_half;

    retval = 0u;
    state = STATE_HIGH;
    while (*source) {
        int error;
        const char lower = (char)tolower(*source++);

        switch (state) {
        case STATE_HIGH: {
            error = hex_to_bin(lower, &hi_half);
            if (error) {
                return retval;
            }
            state = STATE_LOW;
            break;
        }
        case STATE_LOW: {
            uint8_t lo_half;
            error = hex_to_bin(lower, &lo_half);
            if (error) {
                return retval;
            }
            *destination++ = (uint8_t)((hi_half << 4) | lo_half);
            retval++;
            state = STATE_HIGH;
            break;
        }
        default:
            return 0u;
        }
    }
    return retval;
}
