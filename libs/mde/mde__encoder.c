/*
 * mde__encoder.c
 *
 *  Created on: Mar 2, 2021
 *      Author: nenad
 */

#include "mde__encoder.h"

static void encode_lsb_first(const uint16_t scheme[2],
                             uint8_t value,
                             struct mde__encoder__encoded *encoded)
{
    encoded->bits = 0u;

    for (int_fast8_t i = 14; i > -1; i -= 2) {
        uint_fast8_t bv = value & 0x1u;
        encoded->bits |= (uint16_t) (scheme[bv] << i);
        value >>= 1;
    }
}

void mde__encoder__encode__ieee8023(uint8_t value, struct mde__encoder__encoded *encoded)
{
    const uint16_t ieee8023[2] = {
        0x2u, /* zero -> 10*/
        0x1u /* one -> 01 */
    };
    encode_lsb_first(ieee8023, value, encoded);
}

void mde__encoder__encode__ieee8024(uint8_t value, struct mde__encoder__encoded *encoded)
{
    (void) value;
    (void) encoded;
}

void mde__encoder__encode__biphasel(uint8_t value, struct mde__encoder__encoded *encoded)
{
    const uint16_t biphasel[2] = {
        0x2u, /* zero -> 01 -> endian correction -> 10 */
        0x1u /* one -> 10 -> endian correction -> 01 */
    };
    encode_lsb_first(biphasel, value, encoded);
}

uint8_t mde__decoder__decode__ieee8023(const struct mde__encoder__encoded *encoded)
{
    uint_fast8_t value = 0u;
    uint16_t bits = encoded->bits;
    for (int i = 7; i > -1; i--) {
        uint_fast8_t bv = bits & 0x1u;
        value |= bv << i;
        bits >>= 2;
    }
    return (uint8_t) value;
}
