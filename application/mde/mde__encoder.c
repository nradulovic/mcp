/*
 * mde__encoder.c
 *
 *  Created on: Mar 2, 2021
 *      Author: nenad
 */

#include "mde__encoder.h"

void mde__encoder__encode__ieee8023(uint8_t value, struct mde__encoder__encoded *encoded)
{
    const uint16_t manchester_code[2] = {
        0x2u, /* zero */
        0x1u /* one  */
    };
    encoded->bits = 0u;

    for (int i = 14; i > -1; i -= 2) {
        uint_fast8_t bv = value & 0x1u;
        encoded->bits |= manchester_code[bv] << i;
        value >>= 1;
    }
}

void mde__encoder__encode__ieee8024(uint8_t value, struct mde__encoder__encoded *encoded)
{
}

void mde__encoder__encode__biphasel(uint8_t value, struct mde__encoder__encoded *encoded)
{
}

uint8_t mde__decoder__decode__ieee8023(const struct mde__encoder__encoded *encoded)
{
    uint8_t value = 0u;
    uint16_t bits = encoded->bits;
    for (int i = 7; i > -1; i--) {
        uint_fast8_t bv = bits & 0x1u;
        value |= bv << i;
        bits >>= 2;
    }
    return value;
}
