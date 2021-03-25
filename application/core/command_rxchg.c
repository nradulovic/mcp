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

#include "command_rxchg.h"
#include "config_usbd_cdc_terminal.h"
#include "hexador/hexador.h"
#include "mdrv.h"

#define RAW_DATA_SIZE           ((CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE / 2u) + 1u)
#define STRINGIZE(number)        # number
#define DIVIDE_ROUNDUP(numerator, denominator)                                                  \
    (((numerator) + (denominator) - 1u) / (denominator))

const char* command_rxchg__fn(void *terminal_context,
                              void *command_context,
                              size_t arg_count,
                              const char *arg_value[])
{
    struct mdrv__context *mdrv__context = terminal_context;
    const char *arg_data = arg_value[1];
    const char *arg_wr_bit_length = arg_value[2];
    const char *arg_rd_bit_length = arg_value[3];
    size_t data_bits;
    size_t write_bits;
    size_t read_bits;
    uint8_t write_read_data[RAW_DATA_SIZE];
    static char response[RAW_DATA_SIZE * 2u + 5u]; /* Add 3u for \n\r and NULL terminating character + \n\r at the beginning */

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
    data_bits = strlen(arg_data) * 4u;

    /* This check is required by hexador to bin */
    if ((data_bits % 8u) != 0u) {
        static const char *error = "\n\rE0302: Invalid syntax in argument <data>.\n\r"
            "Use HEX notation and 2 values per byte.\n\r";
        return error;
    }
    do {
        long int input = strtol(arg_wr_bit_length, NULL, 10);
        if (input == 0) {
            static const char *error = "\n\rE0304: Zero value of argument <wr_bit_length>.\n\r";
            return error;
        } else if ((input == LONG_MAX) || (input == LONG_MIN) || (input < 0)) {
            static const char *error =
                "\n\rE0305: Value of argument <wr_bit_length> is out of range.\n\r";
            return error;
        } else if (input > (long int) data_bits) {
            static const char *error =
                "\n\rE0306: Value of argument <wr_bit_length> is bigger than <data> bit length.\n\r";
            return error;
        }
        write_bits = (size_t) input;
    } while (0);
    do {
        long int input = strtol(arg_rd_bit_length, NULL, 10);
        if ((input == LONG_MAX) || (input == LONG_MIN) || (input < 0)) {
            static const char *error =
                "\n\rE0308: Value of argument <rd_bit_length> is out of range.\n\r";
            return error;
        }
        read_bits = (size_t) input;
    } while (0);
    size_t raw_data_bytes = hexador__to_bin(arg_data, write_read_data);

    if (raw_data_bytes != (data_bits / 8u)) {
        return "\n\rE0309: Invalid values in argument <data>.\n\r";
    }
    int error = mdrv__xchg(mdrv__context, write_read_data, write_bits, write_read_data, read_bits);

    switch (error) {
    case 0:
        break;
    case -1:
        return "\n\rE0310: Error while executing exchange driver (no enough buffers)\n\r";
    case -2:
        return "\n\rE0311: Failed to convert to Manchester encoding.\n\r";
    case -3:
        return "\n\rE0312: Failed to convert from Manchester encoding.\n\r";
    default:
        return "\n\rE0313: Error while executing exchange driver (unknown reason).\n\r";
    }
    size_t read_bytes = DIVIDE_ROUNDUP(read_bits, 8u);
    response[0] = '\n';
    response[1] = '\r';
    size_t response_bytes = hexador__to_hex(write_read_data, read_bytes, &response[2]);
    if (response_bytes != read_bytes) {
        return "\n\rE0314: Error while converting response to HEX.\n\r";
    }
    response[read_bytes * 2u + 2u + 0u] = '\n';
    response[read_bytes * 2u + 2u + 1u] = '\r';
    response[read_bytes * 2u + 2u + 2u] = '\0';
    return response;
}
