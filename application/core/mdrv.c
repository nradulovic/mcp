
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * mdrv.c
 *
 *  Created on: Mar 3, 2021
 */

#include <string.h>

#include "mdrv.h"
#include "poll_timer.h"
#include "generic/nk_debug.h"
#include "generic/nk_array.h"
#include "generic/nk_types.h"
#include "generic/nk_bits.h"
#include "generic/codec/nk_manchester.h"

#define IO_BUFFER_LENGTH                2048u
#define MIN_PRE_TX_PERIODS              10u

/*
 * Enable debugging of RX or TX processes
 *
 * When you enable RX or TX debug you will be able to see each state start/stop
 * on STATUS GPIO pin.
 */
#define DEBUG_STATUS__TX                0
#define DEBUG_STATUS__RX                1

/*
 * This is an errata of the anyCS chip which was detected in some cases.
 * 
 * It was noticed that anyCS will generate invalid Manchester (both half-bits 
 * are 1-1) code if last bit is '1'. The workaround for this is to ignore last
 * half bit and always generate it based on the first half-bit.
 */
#define ERRATA_IGNORE_LAST_BIT           1

#if ((DEBUG_STATUS__RX == 1) || (DEBUG_STATUS__TX == 1))
#include "stm32f4xx.h"
#include "config_peripherals.h"
#endif

struct mdrv__exc_buffer_bucket
    NK_ARRAY__BUCKET_TYPED_T(bool, IO_BUFFER_LENGTH, struct nk_types__array__bool)
;
struct bit_buffer
    NK_ARRAY__BUCKET_T(uint8_t, NK_BITS__DIVIDE_ROUNDUP(IO_BUFFER_LENGTH, 8u))
;

static struct mdrv__exc_buffer_bucket g__tx__buffer =
        NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&g__tx__buffer)
;
static struct mdrv__exc_buffer_bucket g__rx__buffer =
        NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&g__rx__buffer)
;
static struct bit_buffer g__bit_buffer = NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&g__bit_buffer)
;

/*
 * Default runtime configuration
 */
static const struct mdrv__config g__default_config = {
    .idle = MDRV__CONFIG__IDLE__FLOAT,
    .pre_tx_state = MDRV__CONFIG__PRE_TX__FLOAT,
    .pre_tx_period_count = 512u,
    .quarter_period_us = 4u};

static void set_gpio_to_idle(const struct mdrv__context *context)
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

void mdrv__init(struct mdrv__context *context, const struct mdrv__ll *ll, void *ll_context)
{
    memset(context, 0, sizeof(*context));
    memcpy(&context->p__ll, ll, sizeof(context->p__ll));
    context->p__ll_context = ll_context;
    context->p__config = &g__default_config;
    set_gpio_to_idle(context);
    poll_timer__init();
}

int mdrv__set_config(struct mdrv__context *context, const struct mdrv__config *config)
{
    if (context->p__state != MDRV__STATE__IDLE) {
        return -1;
    }
    context->p__config = config;
    set_gpio_to_idle(context);
    return 0;
}

size_t mdrv__get_io_buffer_length(const struct mdrv__context *context)
{
    (void) context;
    return IO_BUFFER_LENGTH;
}

enum nk_error mdrv__xchg(struct mdrv__context *context,
                         const struct nk_types__array__u8 *wr_data,
                         struct nk_types__array__u8 *rd_data,
                         size_t wr_size,
                         size_t rx_size)
{
    struct nk_manchester__result mnc_result;

    HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_RESET);

    nk_assert(((rx_size != 0u) && (rx_data != NULL)) || ((rx_size == 0u) && (rx_data == NULL)));

    context->p__period_count = 0u;
    context->p__tx_index = 0u;
    context->p__rx_size = rx_size * 2u; /* Every bit has two half-periods */

    g__tx__buffer.array.length = 0u;
    g__rx__buffer.array.length = 0u;

    if ((wr_size * 2u) > g__tx__buffer.array.item_no) {
        return NK_ERROR__BUFFER_OVF;
    }
    if ((rx_size * 2u) > g__rx__buffer.array.item_no) {
        return NK_ERROR__BUFFER_OVF;
    }

    mnc_result = nk_manchester__encode__biphasel(wr_data, &g__tx__buffer.array);

    if (mnc_result.error != NK_ERROR__OK) {
        return NK_ERROR__BUFFER_OVF;
    }
    /* We might specify the wr_size which is smaller than g__tx__buffer length since we want to
     * support bit count writes. The next check ensure that we didn't receive the wr_size bigger
     * than converted bits.
     */
    if ((wr_size * 2u) > g__tx__buffer.array.length) {
        return NK_ERROR__BUFFER_OVF;
    }
    /*
     * Limit the number of bits which needs to be written.
     */
    g__tx__buffer.array.length = wr_size * 2u;

    if (context->p__config->pre_tx_period_count != 0) {
        uint32_t pre_tx_period;
        pre_tx_period = MAX(context->p__config->pre_tx_period_count, MIN_PRE_TX_PERIODS);
        context->p__period_count = pre_tx_period - 1u;
        context->p__state = MDRV__STATE__PRE_TX;
    } else {
        context->p__state = MDRV__STATE__INIT_TX;
    }
    context->p__ll.tim_start(context->p__ll_context, context->p__config->quarter_period_us);
    /*
     * Wait until ISR finishes its job
     */
    while (context->p__state != MDRV__STATE__IDLE) {
        ;
    }
#if (ERRATA_IGNORE_LAST_BIT == 1)
    /*
     * When we have this enabled, we always force the last half-bit to be opposite of the 
     * half-bit before the last one.
     */
    if (g__rx__buffer.array.length >= 2u) {
        g__rx__buffer.array.items[g__rx__buffer.array.length - 1u] =
                            !g__rx__buffer.array.items[g__rx__buffer.array.length - 2u];
    }
#endif
    /* See if everything went OK */
    if ((rx_size != 0u) && (rd_data != NULL)) {
        mnc_result = nk_manchester__decode__biphasel(&g__rx__buffer.array, rd_data);

        switch (mnc_result.error) {
        case NK_ERROR__OK:
            break;
        case NK_ERROR__DATA_ODD:
            return NK_ERROR__DATA_ODD;
        case NK_ERROR__BUFFER_OVF:
            return NK_ERROR__DATA_OVF;
        default:
            return NK_ERROR__DATA_INVALID;
        }
        if (mnc_result.value != rx_size) {
            return NK_ERROR__DATA_UNDERFLOW;
        }
    }
    return 0;
}

/*
 * Force compiler optimization of this function regardles of firmware wide
 * setting.
 */
__attribute__ ((optimize(3)))
void mdrv__it(struct mdrv__context *context)
{
    switch (context->p__state) {
    /* Idle state */
    case MDRV__STATE__IDLE:
        break;
    /* State PRE_TX */
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
             * In this case we didn't initialise output mode so we need to do it at the moment of
             * transmission begin.
             */
            break;
        }
        context->p__state = MDRV__STATE__PRE_TX_COUNT;
        break;
    /* 
     * In this state we are waiting before starting data transmission.
     * 
     * This wait is controlled by p__period_count member.
     */
    case MDRV__STATE__PRE_TX_COUNT:
        context->p__period_count--;
        if (context->p__period_count == 0u) {
            context->p__state = MDRV__STATE__INIT_TX;
        }
        break;
    case MDRV__STATE__INIT_TX:
        /*
         * Write the first bit together with initialisation. Since this is the first bit, we don't
         * need to check if we have reached end of transmission like we do in MDRV__STATE__TX
         */
        context->p__ll.pin_init_output(context->p__ll_context,
                                       g__tx__buffer.array.items[context->p__tx_index++]);
        context->p__state = MDRV__STATE__TX_FHI;
        break;
    /* Send first half-bit */
    case MDRV__STATE__TX_FH:
        context->p__ll.pin_write(context->p__ll_context,
                                 g__tx__buffer.array.items[context->p__tx_index++]);
        context->p__state = MDRV__STATE__TX_FHI;
        break;
    /* Send first half-bit idle period */
    case MDRV__STATE__TX_FHI:
        context->p__state = MDRV__STATE__TX_SH;
        break;
    /* Send second half-bit */
    case MDRV__STATE__TX_SH: {
        context->p__ll.pin_write(context->p__ll_context,
                                 g__tx__buffer.array.items[context->p__tx_index++]);
        context->p__state = MDRV__STATE__TX_SHI;
        break;
    }
    /* Send second half-bit idle period */
    case MDRV__STATE__TX_SHI:
        if (context->p__tx_index == g__tx__buffer.array.length) {
            context->p__state = MDRV__STATE__TX_COMPLETE;
        } else {
            context->p__state = MDRV__STATE__TX_FH;
        }
        break;
    case MDRV__STATE__TX_COMPLETE:
#if (DEBUG_STATUS__RX == 1) && (DEBUG_STATUS__TX == 0)
        HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_RESET);
#endif
        set_gpio_to_idle(context);
        if (context->p__rx_size == 0u) {
            context->p__ll.tim_stop(context->p__ll_context);
            context->p__state = MDRV__STATE__IDLE;
        } else {
            context->p__state = MDRV__STATE__PRE_RX;
        }
        break;
    case MDRV__STATE__PRE_RX:
#if (DEBUG_STATUS__RX == 1)
        HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_SET);
#endif
        context->p__ll.tim_stop(context->p__ll_context);
        context->p__state = MDRV__STATE__RX_FHI;
        context->p__ll.pin_init_input(context->p__ll_context);
        /* Before receiving data we want to detect high-to-low transition.
         * 
         * Since we want fast response, we do the waiting in tight loop where we
         * wait for low state on GPIO pin. In case of faulty anyCS (or an
         * errata) this is the place to check if we got stuck here.
         */
        {
            bool has_timedout = false;
            poll_timer__setup_ms(100);
            while (context->p__ll.pin_read(context->p__ll_context) && !has_timedout) {
                has_timedout = poll_timer__is_done();
            }
            if (has_timedout) {
                set_gpio_to_idle(context);
                context->p__state = MDRV__STATE__IDLE;
            }
            poll_timer__cleanup();
        }
        context->p__ll.tim_start(context->p__ll_context, context->p__config->quarter_period_us);
#if (DEBUG_STATUS__RX == 1)
        HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_RESET);
#endif
        break;
    case MDRV__STATE__RX_FHI: {
#if (DEBUG_STATUS__RX == 1)
        HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_SET);
#endif
        bool value = context->p__ll.pin_read(context->p__ll_context);
        g__rx__buffer.array.items[g__rx__buffer.array.length++] = value;
        context->p__state = MDRV__STATE__RX_FHS;
        break;
    }
    case MDRV__STATE__RX_FHS:
#if (DEBUG_STATUS__RX == 1)
        HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_RESET);
#endif
        context->p__state = MDRV__STATE__RX_SHI;
        break;
    case MDRV__STATE__RX_SHI: {
#if (DEBUG_STATUS__RX == 1)
        HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_SET);
#endif
        bool value = context->p__ll.pin_read(context->p__ll_context);
        g__rx__buffer.array.items[g__rx__buffer.array.length++] = value;
        context->p__state = MDRV__STATE__RX_SHS;
        break;
    }
    case MDRV__STATE__RX_SHS:
#if (DEBUG_STATUS__RX == 1)
        HAL_GPIO_WritePin(MCP_STATUS_GPIO_PORT, MCP_STATUS_PIN, GPIO_PIN_RESET);
#endif
        if (g__rx__buffer.array.length == context->p__rx_size) {
            set_gpio_to_idle(context);
            context->p__ll.tim_stop(context->p__ll_context);
            context->p__state = MDRV__STATE__IDLE;
        } else {
            context->p__state = MDRV__STATE__RX_FHI;
        }
        break;
    /* Invalid state, this should never happen */
    default:
        break;
    }
}

