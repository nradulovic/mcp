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

static size_t byte_array_elements_from_sizeof(size_t sizeof_byte_array)
{
    return sizeof_byte_array / sizeof(uint8_t);
}

static size_t encoded_array_elements_from_sizeof(size_t sizeof_encoded_array)
{
    return sizeof_encoded_array / sizeof(bool);
}

int mde__biphasel__encode_byte_array(const uint8_t *byte_array, size_t byte_array_size,
bool *encoded_array,
                                     size_t encoded_array_size, size_t bits)
{
    size_t d_i;

    if (bits > (byte_array_elements_from_sizeof(byte_array_size) * 8u)) {
        return -1;
    }
    if (bits > encoded_array_elements_from_sizeof(encoded_array_size)) {
        return -1;
    }
    d_i = 0u;
    for (size_t s_i = 0u; s_i < bits; s_i++) {
        size_t byte_i = s_i >> 3u;
        uint_fast8_t bit_mask = 0x80u >> (s_i & 0x7u);
        uint_fast8_t byte = byte_array[byte_i];
        if (byte & bit_mask) {
            encoded_array[d_i++] = true;
            encoded_array[d_i++] = false;
        } else {
            encoded_array[d_i++] = false;
            encoded_array[d_i++] = true;
        }
    }
    return (int) (d_i / 2u);
}

int mde__biphasel__decode_byte_array(const bool *encoded_array,
                                     size_t encoded_array_size,
                                     uint8_t *byte_array,
                                     size_t byte_array_size,
                                     size_t bits)
{
    size_t s_i;

    if (bits > (byte_array_elements_from_sizeof(byte_array_size) * 8u)) {
        return -1;
    }
    if (bits > encoded_array_elements_from_sizeof(encoded_array_size)) {
        return -1;
    }
    s_i = 0u;
    for (size_t d_i = 0u; d_i < bits; d_i++) {
        size_t byte_i = d_i >> 3u;
        size_t bit_i = 0x7u - (d_i & 0x7u);
        /* 10 => 1 */
        if (encoded_array[s_i] && !encoded_array[s_i + 1u]) {
            byte_array[byte_i] |= (uint8_t) (0x1u << bit_i);
        } else if (!encoded_array[s_i] && encoded_array[s_i + 1u]) {
            byte_array[byte_i] &= (uint8_t) ~(0x1u << bit_i);
        } else {
            break;
        }
        s_i += 2u;
    }
    return (int) (s_i / 2u);
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
