/*
 * nk_types.h
 *
 *  Created on: Apr 12, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *
 *  08/05/2021: (nbr) Rearranged the standard types to match standard types from nk_result.h
 */

#ifndef GENERIC_NK_TYPES_H_
#define GENERIC_NK_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nk_array.h"

#if defined(__cplusplus)
extern "C"
{
#endif

struct nk_types__array__bool NK_ARRAY__T(bool);
struct nk_types__array__char NK_ARRAY__T(char);

struct nk_types__array__u8   NK_ARRAY__T(uint8_t);
struct nk_types__array__u16  NK_ARRAY__T(uint16_t);
struct nk_types__array__u32  NK_ARRAY__T(uint32_t);

struct nk_types__array__i8   NK_ARRAY__T(int8_t);
struct nk_types__array__i16  NK_ARRAY__T(int16_t);
struct nk_types__array__i32  NK_ARRAY__T(int32_t);

struct nk_types__array__size NK_ARRAY__T(size_t);
struct nk_types__array__void NK_ARRAY__T(void *);

#if defined(__cplusplus)
}
#endif

#endif /* GENERIC_NK_TYPES_H_ */
