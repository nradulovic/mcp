/*
 * MANCHESTER.h
 *
 *  Created on: Apr 13, 2021
 *      Author: nenad
 */

#ifndef GENERIC_CODEC_NK_MANCHESTER_H_
#define GENERIC_CODEC_NK_MANCHESTER_H_

#include <stddef.h>

#include "generic/nk_result.h"

#define NK_ENABLED_MANCHESTER

struct nk_types__array__u8;
struct nk_types__array__bool;

struct nk_manchester__result NK_RESULT__T(size_t);

struct nk_manchester__result
nk_manchester__encode__biphasel(const struct nk_types__array__u8 *source, struct nk_types__array__bool *encoded);

struct nk_manchester__result
nk_manchester__decode__biphasel(const struct nk_types__array__bool *source, struct nk_types__array__u8 *decoded);

#endif /* GENERIC_CODEC_NK_MANCHESTER_H_ */
