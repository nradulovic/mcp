/*
 * nk_convert.c
 *
 *  Created on: Apr 12, 2021
 *      Author: nenad
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

struct nk_convert__hex_to_bin__result
nk_convert__hex_to_bin(char character)
{
    struct nk_convert__hex_to_bin__result result;

    static const struct hex_map_to_int_array
        NK_FARRAY__T(const struct hex_map_to_int, 16u)
    hex_map = { .items = { { '0', 0 }, { '1', 1 }, { '2', 2 }, { '3', 3 }, { '4', 4 }, { '5', 5 }, { '6', 6 },
                    { '7', 7 }, { '8', 8 }, { '9', 9 }, { 'a', 10 }, { 'b', 11 }, { 'c', 12 }, { 'd', 13 }, { 'e', 14 },
                    { 'f', 15 } } };

    result.error = NK_ERROR__DATA_INVALID;
    result.value = 0u;

    for (size_t i = 0u; i < NK_FARRAY__LENGTH(&hex_map); i++) {
        if (hex_map.items[i].from == nk_char__lower(character)) {
            result.error = NK_ERROR__OK;
            result.value = hex_map.items[i].to;
            break;
        }
    }
    return result;
}

struct nk_convert__bin_to_hex__result
nk_convert__bin_to_hex(uint8_t byte)
{
    struct nk_convert__bin_to_hex__result result;

    static const struct bin_map_to_char_array
        NK_FARRAY__T(const char, 16u)
    bin_map = { .items = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' } };
    if (byte > NK_FARRAY__LENGTH(&bin_map)) {
        result.error = NK_ERROR__DATA_OVF;
        result.value = 0u;
    } else {
        result.error = NK_ERROR__OK;
        result.value = bin_map.items[byte];
    }
    return result;
}

struct nk_convert__str_to_u32__result
nk_convert__str_to_u32(const struct nk_string *string)
{
    struct nk_convert__str_to_u32__result result;
    char null_terminated[1024];
    unsigned long ul;

    if (string->length == (sizeof(null_terminated) - 1u)) {
        result.error = NK_ERROR__DATA_OVF;
        result.value = 0u;
        return result;
    }
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
            result.value = (uint32_t)ul;
        } else {
            result.error = NK_ERROR__DATA_OVF;
            result.value = 0u;
        }
    }

    return result;
}
