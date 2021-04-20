/*
 * hexador.c
 *
 *  Created on: Apr 12, 2021
 *      Author: nenad
 */

#include "hexador.h"
#include "../nk_types.h"
#include "../nk_string.h"
#include "../convert/nk_convert.h"
#include "../nk_debug.h"

struct nk_hexador__result
nk_hexador__to_bin(const struct nk_string *string,
                   struct nk_types__array__u8 *buffer)
{
    struct nk_hexador__result result;

    nk_assert(string);
    nk_assert(buffer);

    if ((string->length % 2u) != 0u) {
        result.error = NK_ERROR__DATA_ODD;
        result.value = 0u;
        return result;
    }
    if ((string->length / 2u) > buffer->length) {
        result.error = NK_ERROR__BUFFER_OVF;
        result.value = 0u;
        return result;
    }
    result.error = NK_ERROR__OK;
    result.value = string->length;
    for (size_t i = 0u; i < string->length; i += 2u) {
        struct nk_convert__hex_to_bin__result htob_result_lo;
        struct nk_convert__hex_to_bin__result htob_result_hi;

        htob_result_hi = nk_convert__hex_to_bin(string->items[i]);
        if (htob_result_hi.error != NK_ERROR__OK) {
            result.error = NK_ERROR__DATA_INVALID;
            result.value = i;
            break;
        }
        htob_result_lo = nk_convert__hex_to_bin(string->items[i + 1]);
        if (htob_result_lo.error != NK_ERROR__OK) {
            result.error = NK_ERROR__DATA_INVALID;
            result.value = i;
            break;
        }
        buffer->items[i / 2u] = (uint8_t) ((htob_result_hi.value << 4) | htob_result_lo.value);
    }
    return result;
}

struct nk_hexador__result
nk_hexador__to_hex(const struct nk_types__array__u8 *buffer,
                   struct nk_string *string)
{
    struct nk_hexador__result result;

    nk_assert(string);
    nk_assert(buffer);

    if (string->length < (buffer->length * 2u)) {
        result.error = NK_ERROR__BUFFER_OVF;
        result.value = 0u;
        return result;
    }
    result.error = NK_ERROR__OK;
    result.value = buffer->length;
    for (size_t i = 0u; i < buffer->length; i++) {
        struct nk_convert__bin_to_hex__result btoh_result;
        uint8_t msb_half = buffer->items[i] >> 4u;
        uint8_t lsb_half = buffer->items[i] & 0xfu;
        btoh_result = nk_convert__bin_to_hex(msb_half);
        if (btoh_result.error != NK_ERROR__OK) {
            result.error = NK_ERROR__DATA_INVALID;
            result.value = i;
            break;
        }
        //string->items[i * 2u] = btoh_result.value;
        btoh_result = nk_convert__bin_to_hex(lsb_half);
        if (btoh_result.error != NK_ERROR__OK) {
            result.error = NK_ERROR__DATA_INVALID;
            result.value = i;
        }
        //string->items[i * 2u + 1u] = btoh_result.value;
    }
    return result;
}

