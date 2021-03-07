/*
 * mdrv.c
 *
 *  Created on: Mar 3, 2021
 *      Author: nenad
 */

#include <string.h>

#include "mdrv.h"
#include "../mde/mde__encoder.h"

#define IO_BUFFER_LENGTH  2048

static struct mde__encoder__encoded io_buffer[IO_BUFFER_LENGTH];

static const struct mdrv__config default_config = {
    .idle = MDRV__CONFIG__IDLE__FLOAT,
    .pre_tx_state = MDRV__CONFIG__PRE_TX__LOW,
    .pre_tx_period_count = 512,
    .tx_period = 10};

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

static bool calculate_bit(const struct mdrv__context *context)
{
    size_t byte_i = context->p__wr_index >> 4;
    size_t bit_i = context->p__wr_index & 0xfu;
    bool value = !!(io_buffer[byte_i].bits & (0x1u << bit_i));

    return value;
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
    const char *value = data;

    if (context->p__state != MDRV__STATE__IDLE) {
        return -1;
    }
    context->p__period_count = 0u;
    context->p__wr_data = data;
    context->p__wr_size = size * 2; /* Manchester encoding uses 2 bits per one data bit. */
    context->p__wr_index = 0u;

    if (size > IO_BUFFER_LENGTH) {
        return -1;
    }
    for (size_t i = 0u; i < size; i++) {
        mde__encoder__encode__biphasel(value[i], &io_buffer[i]);
    }
    if (context->p__config->pre_tx_period_count != 0) {
        uint32_t pre_tx_period = max__uint32_t(context->p__config->pre_tx_period_count, 3u);
        context->p__period_count = pre_tx_period - 1u;
        context->p__state = MDRV__STATE__PRE_TX;
    } else {
        context->p__state = MDRV__STATE__INIT_TX;
    }
    context->p__ll.tim_start(context->p__ll_context, context->p__config->tx_period);
    context->p__wait = true;
    while (context->p__wait) {
        ;
    }

    return 0;
}

#include "main.h"

void mdrv__it(struct mdrv__context *context)
{
    switch (context->p__state) {
    case MDRV__STATE__IDLE:
        break;
    case MDRV__STATE__PRE_TX: {
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
    }
    case MDRV__STATE__PRE_TX_COUNT: {
        context->p__period_count--;
        if (context->p__period_count == 0u) {
            context->p__state = MDRV__STATE__INIT_TX;
        }
        break;
    }
    case MDRV__STATE__INIT_TX: {
        /*
         * Write the first bit together with initialization. Since this is the first bit, we don't
         * need to check if we have reached end of transmission like we do in MDRV__STATE__TX
         */
        context->p__ll.pin_init_output(context->p__ll_context, calculate_bit(context));
        context->p__wr_index++;
        context->p__state = MDRV__STATE__TX;
        HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_SET);
        break;
    }
    case MDRV__STATE__TX: {
        if (context->p__wr_index & 0x1u) {
            HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_SET);
        }
        context->p__ll.pin_write(context->p__ll_context, calculate_bit(context));
        context->p__wr_index++;
        if (context->p__wr_index == context->p__wr_size) {
            context->p__state = MDRV__STATE__TX_COMPLETE;
        }
        break;
    }
    case MDRV__STATE__TX_COMPLETE: {
        set_idle(context);
        context->p__ll.tim_stop(context->p__ll_context);
        context->p__state = MDRV__STATE__IDLE;
        context->p__wait = false;
        /* Call the complete handler here */
    }
    default:
        break;
    }
}
