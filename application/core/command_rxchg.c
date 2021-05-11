/*
 * command_rxchg.c
 *
 *  Created on: Mar 10, 2021
 *      Author: nenad
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "generic/nk_array.h"
#include "generic/nk_string.h"
#include "generic/nk_types.h"
#include "generic/convert/nk_convert.h"
#include "generic/convert/nk_hexador.h"

#include "command_rxchg.h"
#include "config_usbd_cdc_terminal.h"
#include "usbd_cdc_terminal.h"
#include "mdrv.h"

#define RAW_DATA_SIZE           ((CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE / 2u) + 1u)
#define STRINGIZE(number)        # number
#define DIVIDE_ROUNDUP(numerator, denominator)                                                  \
    (((numerator) + (denominator) - 1u) / (denominator))

#define ARG0_DATA   1
#define ARG1_WR_BIT_LENGTH 2
#define ARG2_RD_BIT_LENGTH 3

struct raw_data_bucket
    NK_ARRAY__BUCKET_TYPED_T(uint8_t, RAW_DATA_SIZE, struct nk_types__array__u8)
;

const char* command_rxchg__fn(void *terminal_context,
                              void *command_context,
                              struct terminal_arguments *args,
                              struct nk_string *output)
{
    struct usbd_cdc_terminal__context *context = terminal_context;
    struct mdrv__context *mdrv__context = context->mdrv;
    size_t data_bits;
    size_t write_bits;
    size_t read_bits;
    enum nk_error error;
    struct raw_data_bucket raw_write_data = NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&raw_write_data)
    ;
    struct raw_data_bucket raw_read_data = NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&raw_read_data)
    ;

    (void) command_context;

    if (args->length == 1) {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL(
                        "\n\rRaw Exchange Data (rxchg) command syntax:\n\r"
                        "\n\rrxchg <data> <wr_bit_length> <rd_bit_length>\n\r"
                        "\n\r"
                        "  data          - Data array of 8-bit values written in HEX notation.\n\r"
                        "  wr_bit_length - Number of bits to actually write in DEC notation.\n\r"
                        "  rd_bit_length - Number of bits to actually read in DEC notation.\n\r"));
        return NULL;
    }
    if (args->length != 4) {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0300: Incorrect number of arguments.\n\r"));
        return NULL;
    }
    data_bits = args->items[ARG0_DATA].length * 4u;

    /* This check is required by hexador to bin */
    if ((data_bits % 8u) != 0u) {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0302: Invalid syntax in argument <data>.\n\r"
                                   "Use HEX notation and 2 values per byte.\n\r"));
        return NULL;
    }
    struct nk_result__u32 str_to_u32_result;

    str_to_u32_result = nk_convert__str_to_u32(&args->items[ARG1_WR_BIT_LENGTH]);
    switch (str_to_u32_result.error) {
    case NK_ERROR__OK:
        write_bits = str_to_u32_result.value;
        break;
    default: {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL(
                        "\n\rE0305: Value of argument <wr_bit_length> is out of range.\n\r"));
        return NULL;
    }
    }
    if (write_bits == 0u) {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0304: Zero value of argument <wr_bit_length>.\n\r"));
        return NULL;
    }
    if (write_bits > data_bits) {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0306: Value of argument <wr_bit_length> is bigger"
                                   " than <data> bit length.\n\r"));
        return NULL;
    }
    str_to_u32_result = nk_convert__str_to_u32(&args->items[ARG2_RD_BIT_LENGTH]);
    switch (str_to_u32_result.error) {
    case NK_ERROR__OK:
        read_bits = str_to_u32_result.value;
        break;
    default: {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL(
                        "\n\rE0308: Value of argument <rd_bit_length> is out of range.\n\r"));
        return NULL;
    }
    }
    struct nk_hexador__result hexador_result = nk_hexador__to_bin(&args->items[ARG0_DATA],
                                                                  &raw_write_data.array);

    switch (hexador_result.error) {
    case NK_ERROR__OK:
        break;
    case NK_ERROR__BUFFER_OVF:
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL(
                        "\n\rE0320: Error while converting HEX data (buffer overflow)\n\r"));
        return NULL;
    default:
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0321: Error while converting HEX data\n\r"));
        return NULL;
    }
    if (hexador_result.value != args->items[ARG0_DATA].length) {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0309: Invalid values in argument <data>.\n\r"));
        return NULL;
    }
    {
        struct nk_types__array__u8 raw_write_data_view;
        NK_ARRAY__INITIALIZE_WINDOW(&raw_write_data_view, &raw_write_data.array, 0, write_bits);
        error = mdrv__xchg(mdrv__context, &raw_write_data_view, &raw_read_data.array, read_bits);
    }

    switch (error) {
    case NK_ERROR__OK:
        break;
    case NK_ERROR__BUFFER_OVF:
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL(
                        "\n\rE0310: Error while executing exchange driver (no enough buffers)\n\r"));
        return NULL;
    case NK_ERROR__DATA_ODD:
    case NK_ERROR__DATA_INVALID:
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0311: Failed to convert to Manchester encoding.\n\r"));
        return NULL;
    case NK_ERROR__DATA_OVF:
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0312: Failed to convert from Manchester encoding.\n\r"));
        return NULL;
    default:
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL(
                        "\n\rE0313: Error while executing exchange driver (unknown reason).\n\r"));
        return NULL;
    }
    nk_string__append_literal(output, NK_STRING__LITERAL("\n\r"));
    hexador_result = nk_hexador__to_hex(&raw_read_data.array, output);

    if (hexador_result.error != NK_ERROR__OK) {
        nk_string__append_literal(
                output,
                NK_STRING__LITERAL("\n\rE0314: Error while converting response to HEX.\n\r"));
        return NULL;
    }
    nk_string__append_literal(output, NK_STRING__LITERAL("\n\r"));
    return NULL;
}
