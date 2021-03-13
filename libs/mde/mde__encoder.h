/*
 * mde__encoder.h
 *
 *  Created on: Mar 2, 2021
 *      Author: nenad
 */

#ifndef MDE__ENCODER_H_
#define MDE__ENCODER_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct mde__encoded_bits
{
    bool halves[2];
};

struct mde__encoder__encoded
{
    uint16_t bits;
};

int mde__biphasel__encode_byte_array(const uint8_t *byte_array,
                                     size_t byte_array_size,
                                     bool *encoded_array,
                                     size_t encoded_array_size,
                                     size_t bits);

int mde__biphasel__decode_byte_array(const bool * encoded_array,
                                     size_t encoded_array_size,
                                     uint8_t *byte_array,
                                     size_t byte_array_size,
                                     size_t bits);

void mde__encoder__encode__ieee8023(uint8_t value, struct mde__encoder__encoded *encoded);
void mde__encoder__encode__ieee8024(uint8_t value, struct mde__encoder__encoded *encoded);
void mde__encoder__encode__biphasel(uint8_t value, struct mde__encoder__encoded *encoded);

uint8_t mde__decoder__decode__ieee8023(const struct mde__encoder__encoded *encoded);

#endif /* MDE__ENCODER_H_ */
