/*
 * mdrv.c
 *
 *  Created on: Mar 3, 2021
 *      Author: nenad
 */

#include <string.h>

#include "mdrv.h"
#include "../mde/mde__encoder.h"

static struct mde__encoder__encoded io_buffer[2048];

void mdrv__init(struct mdrv__context *context, const struct mdrv__ll *ll, void *ll_context)
{
    memset(context, 0, sizeof(*context));
    memcpy(&context->p__ll, ll, sizeof(context->p__ll));
    context->p__ll_context = ll_context;
}

void mdrv__write(struct mdrv__context *context, const void *data, size_t size)
{
    const char *value = data;

    context->p__state = MDRV__STATE__WRITE;
    context->p__wr_data = data;
    context->p__wr_size = size * 16;
    context->p__wr_index = 0u;

    for (size_t i = 0u; i < size; i++) {
        mde__encoder__encode__ieee8023(value[i], &io_buffer[i]);
    }
    context->p__ll.pin_init_output(context->p__ll_context);
    context->p__ll.tim_start(context->p__ll_context);
}

void mdrv__it(struct mdrv__context *context)
{
    switch (context->p__state) {
    case MDRV__STATE__IDLE:
        break;
    case MDRV__STATE__WRITE: {
        size_t byte_i = context->p__wr_index >> 4;
        size_t bit_i = context->p__wr_index & 0xfu;
        context->p__ll.pin_write(context->p__ll_context, !!(io_buffer[byte_i].bits & (0x1u << bit_i)));
        context->p__wr_index++;
        if (context->p__wr_index == context->p__wr_size) {
            context->p__state = MDRV__STATE__IDLE;
            context->p__ll.tim_stop(context->p__ll_context);
            context->p__ll.pin_init_input(context->p__ll_context);
        }
        break;
    }
    default:
        break;
    }
}
