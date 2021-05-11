/*
 * nk_convert.h
 *
 *  Created on: Apr 12, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *
 *  08/05/2021: (nbr) Hex functions will convert whole binary byte.
 */

#ifndef GENERIC_CONVERT_NK_CONVERT_H_
#define GENERIC_CONVERT_NK_CONVERT_H_

#include <stdint.h>

#include "generic/nk_result.h"

#define NK_ENABLED_CONVERT

struct nk_convert__hex
{
    char msb;
    char lsb;
};

struct nk_result__u8
nk_convert__hex_to_bin(struct nk_convert__hex hex);

struct nk_convert__hex
nk_convert__bin_to_hex(uint8_t byte);

struct nk_result__u32
nk_convert__str_to_u32(const struct nk_string * string);

#endif /* GENERIC_CONVERT_NK_CONVERT_H_ */
