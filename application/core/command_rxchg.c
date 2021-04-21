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
#include "generic/convert/hexador.h"

#include "command_rxchg.h"
#include "config_usbd_cdc_terminal.h"
#include "usbd_cdc_terminal.h"
#include "mdrv.h"

#define RAW_DATA_SIZE           ((CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE / 2u) + 1u)
#define STRINGIZE(number)        # number
#define DIVIDE_ROUNDUP(numerator, denominator)                                                  \
    (((numerator) + (denominator) - 1u) / (denominator))

struct raw_data_bucket
    NK_ARRAY__BUCKET_TYPED_T(uint8_t, RAW_DATA_SIZE, struct nk_types__array__u8)
;

const char* command_rxchg__fn(void *terminal_context,
                              void *command_context,
                              size_t arg_count,
                              const char *arg_value[])
{
    struct usbd_cdc_terminal__context *context = terminal_context;
    struct mdrv__context *mdrv__context = context->mdrv;
    struct big_argument
        NK_STRING__BUCKET_T(100)
    arg_data;
    struct small_argument
        NK_STRING__BUCKET_T(10)
    arg_wr_bit_length;
    struct small_argument arg_rd_bit_length;
    size_t data_bits;
    size_t write_bits;
    size_t read_bits;
    enum nk_error error;
    struct raw_data_bucket raw_write_data = NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&raw_write_data)
    ;
    struct raw_data_bucket raw_read_data = NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&raw_read_data)
    ;
    /* Needs to be static since it is returned by this function */
    static struct response_type
        NK_STRING__BUCKET_T(RAW_DATA_SIZE)
    response;

    (void) command_context;

    if (arg_count == 1) {
        static const char *help = "\n\rRaw Exchange Data (rxchg) command syntax:\n\r"
            "\n\rrxchg <data> <wr_bit_length> <rd_bit_length>\n\r"
            "\n\r"
            "  data          - Data array of 8-bit values written in HEX notation.\n\r"
            "  wr_bit_length - Number of bits to actually write in DEC notation.\n\r"
            "  rd_bit_length - Number of bits to actually read in DEC notation.\n\r";
        return help;
    }
    if (arg_count != 4) {
        static const char *error = "\n\rE0300: Incorrect number of arguments.\n\r";
        return error;
    }
    NK_STRING__BUCKET_INITIALIZE_EMPTY(&response);
    NK_STRING__BUCKET_INITIALIZE(&arg_data, arg_value[1], strlen(arg_value[1]));
    NK_STRING__BUCKET_INITIALIZE(&arg_wr_bit_length, arg_value[2], strlen(arg_value[2]));
    NK_STRING__BUCKET_INITIALIZE(&arg_rd_bit_length, arg_value[3], strlen(arg_value[3]));
    data_bits = arg_data.array.length * 4u;

    /* This check is required by hexador to bin */
    if ((data_bits % 8u) != 0u) {
        static const char *error = "\n\rE0302: Invalid syntax in argument <data>.\n\r"
            "Use HEX notation and 2 values per byte.\n\r";
        return error;
    }
    struct nk_convert__str_to_u32__result str_to_u32_result;

    str_to_u32_result = nk_convert__str_to_u32(&arg_wr_bit_length.array);
    switch (str_to_u32_result.error) {
    case NK_ERROR__OK:
        write_bits = str_to_u32_result.value;
        break;
    default: {
        static const char *error =
            "\n\rE0305: Value of argument <wr_bit_length> is out of range.\n\r";
        return error;
    }
    }
    if (write_bits == 0u) {
        static const char *error = "\n\rE0304: Zero value of argument <wr_bit_length>.\n\r";
        return error;
    }
    if (write_bits > data_bits) {
        static const char *error =
            "\n\rE0306: Value of argument <wr_bit_length> is bigger than <data> bit length.\n\r";
        return error;
    }
    str_to_u32_result = nk_convert__str_to_u32(&arg_rd_bit_length.array);
    switch (str_to_u32_result.error) {
    case NK_ERROR__OK:
        read_bits = str_to_u32_result.value;
        break;
    default: {
        static const char *error =
            "\n\rE0308: Value of argument <rd_bit_length> is out of range.\n\r";
        return error;
    }
    }
    struct nk_hexador__result hexador_result = nk_hexador__to_bin(&arg_data.array,
                                                                  &raw_write_data.array);

    switch (hexador_result.error) {
    case NK_ERROR__OK:
        break;
    case NK_ERROR__BUFFER_OVF:
        return "\n\rE0320: Error while converting HEX data (buffer overflow)\n\r";
    default:
        return "\n\rE0321: Error while converting HEX data\n\r";
    }
    if (hexador_result.value != arg_data.array.length) {
        return "\n\rE0309: Invalid values in argument <data>.\n\r";
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
        return "\n\rE0310: Error while executing exchange driver (no enough buffers)\n\r";
    case NK_ERROR__DATA_ODD:
    case NK_ERROR__DATA_INVALID:
        return "\n\rE0311: Failed to convert to Manchester encoding.\n\r";
    case NK_ERROR__DATA_OVF:
        return "\n\rE0312: Failed to convert from Manchester encoding.\n\r";
    default:
        return "\n\rE0313: Error while executing exchange driver (unknown reason).\n\r";
    }
    response.array.items[response.array.length++] = '\n';
    response.array.items[response.array.length++] = '\r';
    hexador_result = nk_hexador__to_hex(&raw_read_data.array, &response.array);

    if (hexador_result.error != NK_ERROR__OK) {
        return "\n\rE0314: Error while converting response to HEX.\n\r";
    }
    response.array.items[response.array.length++] = '\n';
    response.array.items[response.array.length++] = '\r';
    response.array.items[response.array.length++] = '\0'; /* Terminate the array in C string style */
    return response.array.items;
}
