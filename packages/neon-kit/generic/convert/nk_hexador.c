/*
 * nk_hexador.c
 *
 *  Created on: Apr 12, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *  08/05/2021: (nbr) Minor code formatting
 */

#include "generic/nk_debug.h"
#include "generic/nk_types.h"
#include "generic/nk_string.h"
#include "generic/convert/nk_hexador.h"
#include "generic/convert/nk_convert.h"


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
        struct nk_result__u8 htob_result;
        struct nk_convert__hex hex;

        hex.lsb = string->items[i];
        hex.msb = string->items[i + 1];
        htob_result = nk_convert__hex_to_bin(hex);
        if (htob_result.error != NK_ERROR__OK) {
            result.error = NK_ERROR__DATA_INVALID;
            result.value = i;
            break;
        }
        buffer->items[i / 2u] = htob_result.value;
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

    for (size_t i = 0u; i < buffer->length; i++) {
        struct nk_convert__hex btoh_result = nk_convert__bin_to_hex(buffer->items[i]);
        string->items[i * 2u] = btoh_result.lsb;
        string->items[i * 2u + 1u] = btoh_result.msb;
    }
    result.error = NK_ERROR__OK;
    result.value = buffer->length;
    return result;
}

