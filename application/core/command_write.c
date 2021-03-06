/*
 * command_help.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include "command_write.h"
#include "mdrv.h"

const char* command_write__fn(void *terminal_context,
                              void *command_context,
                              size_t arg_count,
                              const char *arg_value[])
{
    struct mdrv__context * mdrv__context = terminal_context;

    (void)arg_value;

    if (arg_count != 2) {
        static const char * help = "\n\rUsage:\n\r"
            "write <data>\n\r"
            "\n\r data - Data array written in HEX notation\n\r";
        return help;
    }
    const char test_data[5] = { 0x1, 0x2, 0x3, 0x4, 0x5};

    (void) command_context;
    mdrv__write(mdrv__context, test_data, sizeof(test_data));

    return NULL;
}
