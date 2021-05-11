/*
 * nk_result.h
 *
 *  Created on: Apr 11, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *
 *  08/05/2021: (nbr) Added standard types
 */

#ifndef NEON_KIT_GENERIC_NK_RESULT_H_
#define NEON_KIT_GENERIC_NK_RESULT_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nk_error.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define NK_RESULT__T(value_type)                                            \
        {                                                                   \
            enum nk_error error;                                            \
            value_type value;                                               \
        }

struct nk_result__bool NK_RESULT__T(bool);
struct nk_result__char NK_RESULT__T(char);

struct nk_result__u8   NK_RESULT__T(uint8_t);
struct nk_result__u16  NK_RESULT__T(uint16_t);
struct nk_result__u32  NK_RESULT__T(uint32_t);

struct nk_result__i8   NK_RESULT__T(int8_t);
struct nk_result__i16  NK_RESULT__T(int16_t);
struct nk_result__i32  NK_RESULT__T(int32_t);

struct nk_result__size NK_RESULT__T(size_t);
struct nk_result__void NK_RESULT__T(void *);

#if defined(__cplusplus)
}
#endif

#endif /* NEON_KIT_GENERIC_NK_RESULT_H_ */
