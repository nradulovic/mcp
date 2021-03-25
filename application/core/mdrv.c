/*
 * mdrv.c
 *
 *  Created on: Mar 3, 2021
 *      Author: nenad
 */

#include <string.h>

#include "mdrv.h"
#include "mde/mde__encoder.h"

#define IO_BUFFER_LENGTH                2048u
#define MIN_PRE_TX_PERIODS              10u

static bool g__tx__encoded_bits[IO_BUFFER_LENGTH * 2u];
static bool g__rx__encoded_bits[IO_BUFFER_LENGTH * 2u];

static const struct mdrv__config default_config = {
    .idle = MDRV__CONFIG__IDLE__FLOAT,
    .pre_tx_state = MDRV__CONFIG__PRE_TX__LOW,
    .pre_tx_period_count = 512u,
    .quarter_period_us = 5u};

static void set_idle(const struct mdrv__context *context)
{
    switch (context->p__config->idle) {
    case MDRV__CONFIG__IDLE__HIGH:
        context->p__ll.pin_write(context->p__ll_context, true);
        break;
    case MDRV__CONFIG__IDLE__LOW:
        context->p__ll.pin_write(context->p__ll_context, false);
        break;
    default:
        context->p__ll.pin_init_input(context->p__ll_context);
        break;
    }
}

static bool get_half_bit(size_t bit_index)
{
    return g__tx__encoded_bits[bit_index];
}

static void put_half_bit(size_t bit_index, bool value)
{
    g__rx__encoded_bits[bit_index] = value;
}

static uint32_t max__uint32_t(uint32_t v1, uint32_t v2)
{
    return v1 > v2 ? v1 : v2;
}

void mdrv__init(struct mdrv__context *context, const struct mdrv__ll *ll, void *ll_context)
{
    memset(context, 0, sizeof(*context));
    memcpy(&context->p__ll, ll, sizeof(context->p__ll));
    context->p__ll_context = ll_context;
    context->p__config = &default_config;
    set_idle(context);
}

int mdrv__set_config(struct mdrv__context *context, const struct mdrv__config *config)
{
    if (context->p__state != MDRV__STATE__IDLE) {
        return -1;
    }
    context->p__config = config;
    set_idle(context);
    return 0;
}

size_t mdrv__get_io_buffer_length(const struct mdrv__context *context)
{
    (void) context;
    return IO_BUFFER_LENGTH;
}

int mdrv__write(struct mdrv__context *context, const void *data, size_t size)
{
    if (size > IO_BUFFER_LENGTH) {
        return -1;
    }
    context->p__period_count = 0u;
    context->p__rd_data = NULL;
    context->p__rd_size = 0u;
    context->p__wr_data = data;
    context->p__wr_size = size * 2; /* Manchester encoding uses 2 bits per one data bit. */
    context->p__index = 0u;
    memset(g__tx__encoded_bits, 0, sizeof(g__tx__encoded_bits));
    int converted = mde__biphasel__encode_byte_array(data,
                                                     size,
                                                     g__tx__encoded_bits,
                                                     sizeof(g__tx__encoded_bits),
                                                     size);

    if (converted != (int)size) {
        return -2;
    }
    if (context->p__config->pre_tx_period_count != 0u) {
        uint32_t pre_tx_period;
        pre_tx_period = max__uint32_t(context->p__config->pre_tx_period_count, MIN_PRE_TX_PERIODS);
        context->p__period_count = pre_tx_period - 1u;
        context->p__state = MDRV__STATE__PRE_TX;
    } else {
        context->p__state = MDRV__STATE__INIT_TX;
    }
    context->p__ll.tim_start(context->p__ll_context, context->p__config->quarter_period_us);
    while (context->p__state != MDRV__STATE__IDLE) {
        ;
    }

    return 0;
}

int mdrv__xchg(struct mdrv__context *context,
               const void *wr_data,
               size_t wr_size,
               void *rd_data,
               size_t rd_size)
{
    if ((wr_size > IO_BUFFER_LENGTH) || (rd_size > IO_BUFFER_LENGTH)) {
        return -1;
    }
    context->p__period_count = 0u;
    context->p__rd_data = rd_data;
    context->p__rd_size = rd_size * 2u; /* Manchester encoding uses 2 bits per one data bit. */
    context->p__wr_data = wr_data;
    context->p__wr_size = wr_size * 2u; /* Manchester encoding uses 2 bits per one data bit. */
    context->p__index = 0u;

    memset(g__tx__encoded_bits, 0, sizeof(g__tx__encoded_bits));
    memset(g__rx__encoded_bits, 0, sizeof(g__rx__encoded_bits));
    int converted = mde__biphasel__encode_byte_array(wr_data,
                                                     wr_size,
                                                     g__tx__encoded_bits,
                                                     sizeof(g__tx__encoded_bits),
                                                     wr_size);

    if (converted != (int)wr_size) {
        return -2;
    }
    if (context->p__config->pre_tx_period_count != 0) {
        uint32_t pre_tx_period;
        pre_tx_period = max__uint32_t(context->p__config->pre_tx_period_count, MIN_PRE_TX_PERIODS);
        context->p__period_count = pre_tx_period - 1u;
        context->p__state = MDRV__STATE__PRE_TX;
    } else {
        context->p__state = MDRV__STATE__INIT_TX;
    }
    context->p__ll.tim_start(context->p__ll_context, context->p__config->quarter_period_us);
    while (context->p__state != MDRV__STATE__IDLE) {
        ;
    }
    converted = mde__biphasel__decode_byte_array(g__rx__encoded_bits,
                                                sizeof(g__rx__encoded_bits),
                                                rd_data,
                                                rd_size,
                                                rd_size);

    if (converted != (int)rd_size) {
        return -3;
    }
    return 0;
}

#include "main.h"

void mdrv__it(struct mdrv__context *context)
{
    switch (context->p__state) {
    case MDRV__STATE__IDLE:
        break;
    case MDRV__STATE__PRE_TX:
        switch (context->p__config->pre_tx_state) {
        case MDRV__CONFIG__PRE_TX__HIGH:
            context->p__ll.pin_init_output(context->p__ll_context, true);
            break;
        case MDRV__CONFIG__PRE_TX__LOW:
            context->p__ll.pin_init_output(context->p__ll_context, false);
            break;
        default:
            /*
             * In this case we didn't initialize output mode so we need to do it at the moment of
             * transmission begin.
             */
            break;
        }
        context->p__state = MDRV__STATE__PRE_TX_COUNT;
        break;
    case MDRV__STATE__PRE_TX_COUNT:
        context->p__period_count--;
        if (context->p__period_count == 0u) {
            context->p__state = MDRV__STATE__INIT_TX;
        }
        break;
    case MDRV__STATE__INIT_TX:
        /*
         * Write the first bit together with initialization. Since this is the first bit, we don't
         * need to check if we have reached end of transmission like we do in MDRV__STATE__TX
         */
        context->p__ll.pin_init_output(context->p__ll_context, get_half_bit(context->p__index));
        context->p__index++;
        context->p__state = MDRV__STATE__TX_FHI;
        break;
    case MDRV__STATE__TX_FH:
        context->p__ll.pin_write(context->p__ll_context, get_half_bit(context->p__index));
        context->p__index++;
        context->p__state = MDRV__STATE__TX_FHI;
        break;
    case MDRV__STATE__TX_FHI:
        context->p__state = MDRV__STATE__TX_SH;
        break;
    case MDRV__STATE__TX_SH: {
        context->p__ll.pin_write(context->p__ll_context, get_half_bit(context->p__index));
        context->p__index++;
        context->p__state = MDRV__STATE__TX_SHI;
        break;
    }
    case MDRV__STATE__TX_SHI:
        if (context->p__index == context->p__wr_size) {
            context->p__state = MDRV__STATE__TX_COMPLETE;
        } else {
            context->p__state = MDRV__STATE__TX_FH;
        }
        break;
    case MDRV__STATE__TX_COMPLETE:
        if (context->p__rd_size == 0u) {
            set_idle(context);
            context->p__ll.tim_stop(context->p__ll_context);
            context->p__state = MDRV__STATE__IDLE;
        } else {
            context->p__state = MDRV__STATE__PRE_RX;
        }
        break;
    case MDRV__STATE__PRE_RX:
        context->p__ll.tim_stop(context->p__ll_context);
        context->p__index = 0u;
        context->p__state = MDRV__STATE__RX_FHI;
        context->p__ll.pin_init_input(context->p__ll_context);
        while (context->p__ll.pin_read(context->p__ll_context))
            ;
        context->p__ll.tim_start(context->p__ll_context, context->p__config->quarter_period_us);
        break;
    case MDRV__STATE__RX_FHI: {
        bool value = context->p__ll.pin_read(context->p__ll_context);
        put_half_bit(context->p__index, value);
        context->p__index++;
        context->p__state = MDRV__STATE__RX_FHS;
        break;
    }
    case MDRV__STATE__RX_FHS:

        context->p__state = MDRV__STATE__RX_SHI;
        break;
    case MDRV__STATE__RX_SHI: {
        bool value = context->p__ll.pin_read(context->p__ll_context);
        put_half_bit(context->p__index, value);
        context->p__index++;
        context->p__state = MDRV__STATE__RX_SHS;
        break;
    }
    case MDRV__STATE__RX_SHS:
        if (context->p__index == context->p__rd_size) {
            set_idle(context);
            context->p__ll.tim_stop(context->p__ll_context);
            context->p__state = MDRV__STATE__IDLE;
        } else {
            context->p__state = MDRV__STATE__RX_FHI;
        }
        break;
    default:
        break;
    }
}
