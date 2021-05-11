/*
 * nk_convert.c
 *
 *  Created on: Apr 12, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *  08/05/2021: (nbr) Hex functions will convert whole binary byte.
 *                    Using variable length array in str_to_u32
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "generic/nk_farray.h"
#include "generic/nk_string.h"
#include "nk_convert.h"

struct hex_map_to_int
{
    char from;
    uint8_t to;
};

static uint8_t
to_bin(char hex_char)
{
    uint8_t result = UINT8_MAX;

    static const struct hex_map_to_int_array
        NK_FARRAY__T(const struct hex_map_to_int, 16u)
    hex_map = { .items = { { '0', 0 }, { '1', 1 }, { '2', 2 }, { '3', 3 }, { '4', 4 }, { '5', 5 }, { '6', 6 },
                    { '7', 7 }, { '8', 8 }, { '9', 9 }, { 'a', 10 }, { 'b', 11 }, { 'c', 12 }, { 'd', 13 }, { 'e', 14 },
                    { 'f', 15 } } };

    for (size_t i = 0u; i < NK_FARRAY__LENGTH(&hex_map); i++) {
        if (hex_map.items[i].from == nk_char__lower(hex_char)) {
            result = hex_map.items[i].to;
            break;
        }
    }
    return result;
}

struct nk_result__u8
nk_convert__hex_to_bin(struct nk_convert__hex hex)
{
    struct nk_result__u8 result;
    uint8_t lsb;
    uint8_t msb;

    result.error = NK_ERROR__DATA_INVALID;
    result.value = 0u;

    lsb = to_bin(hex.lsb);
    if (lsb == UINT8_MAX) {
        return result;
    }
    msb = to_bin(hex.msb);
    if (msb == UINT8_MAX) {
        return result;
    }
    result.error = NK_ERROR__OK;
    result.value = (uint8_t) ((msb << 4u) | lsb);
    return result;
}

struct nk_convert__hex
nk_convert__bin_to_hex(uint8_t byte)
{
    struct nk_convert__hex result;

    static const struct bin_map_to_char_array
        NK_FARRAY__T(const char, 16u)
    bin_map = { .items = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' } };
    result.lsb = bin_map.items[byte & 0xfu];
    result.msb = bin_map.items[(byte >> 4u) & 0xfu];
    return result;
}

struct nk_result__u32
nk_convert__str_to_u32(const struct nk_string *string)
{
    struct nk_result__u32 result;
    unsigned long ul;
    /* When creating a standard C string add one more place for null terminating character.
     */
    char null_terminated[string->length + 1u];

    memcpy(null_terminated, string->items, string->length);
    null_terminated[string->length] = '\0';
    errno = 0;
    ul = strtoul(null_terminated, NULL, 10);
    if (errno != 0) {
        result.error = NK_ERROR__DATA_INVALID;
        result.value = 0u;
    } else {
        if (ul < UINT32_MAX) {
            result.error = NK_ERROR__OK;
            result.value = (uint32_t) ul;
        } else {
            result.error = NK_ERROR__DATA_OVF;
            result.value = 0u;
        }
    }

    return result;
}
