/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "command_write.h"
#include "config_usbd_cdc_terminal.h"
#include "mdrv.h"
#include "hexador/hexador.h"

#define STRINGIZE(number)  # number

#define RAW_DATA_SIZE           ((CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE / 2u) + 1u)

const char* command_write__fn(void *terminal_context,
                              void *command_context,
                              size_t arg_count,
                              const char *arg_value[])
{
    struct mdrv__context *mdrv__context = terminal_context;
    const char *write_data = arg_value[1];
    uint8_t raw_data[RAW_DATA_SIZE];
    size_t write_data_length;
    size_t write_data_bit_length;

    (void) command_context;

    if ((arg_count != 2) && (arg_count != 3)) {
        static const char *help = "\n\rE0200: Incorrect number of arguments.\n\r"
            "\n\rwrite <data>\n\r"
            "  data       - Data array of 8-bit values written in HEX notation.\n\r"
            "\n\rwrite <data> <bit_length>\n\r"
            "  data       - Data array of 8-bit values written in HEX notation.\n\r"
            "  bit_length - Number of bits to actually write in DEC notation.\n\r";
        return help;
    }
    write_data_length = strlen(write_data);

    if (write_data_length >= CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE) {
        static const char *error = "\n\rE0201: Argument <value> length is too big.\n\r"
            "Maximum argument length is " STRINGIZE(CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE)
        " characters\n\r";
        return error;
    }
    if (write_data_length & 0x1u) {
        static const char *error = "\n\rE0202: Invalid syntax in argument <value>.\n\r"
            "Use HEX notation and 2 values per byte.\n\r";
        return error;
    }
    if (arg_count == 3) {
        long int input = strtol(arg_value[2], NULL, 10);
        if (input == 0) {
            static const char *error = "\n\rE0205: Zero value of argument <bit_length>.\n\r";
            return error;
        } else if ((input == LONG_MAX) || (input == LONG_MIN) || (input < 0)) {
            static const char *error =
                "\n\rE0206: Value of argument <bit_length> is out of range.\n\r";
            return error;
        } else if (input > (long int)(write_data_length * 8u)) {
            static const char *error =
                "\n\rE0207: Value of argument <bit_length> is bigger than <value> length.\n\r";
            return error;
        }
        write_data_bit_length = (size_t) input;
    } else {
        write_data_bit_length = write_data_length * 8u;
    }
    size_t raw_data_length = hexador__to_bin(write_data, raw_data);

    if (raw_data_length != (write_data_length / 2u)) {
        return "\n\rE0203: Invalid values in argument <value>.\n\r";
    }
    if (mdrv__write(mdrv__context, raw_data, write_data_bit_length)) {
        return "\n\rE0204: Error while executing driver write command.\n\r";
    }

    return "\n\r";
}
