
/*
 * Copyright (C) IngBiro - Jovan Jovanovic
 * 
 * +381 63 477858 Jovan Jovanovic (info@ingbiro.net)
 * Vizantijski Bulevar 98/12
 */
/*
 * mdrv.h
 *
 *  Created on: Mar 3, 2021
 */

#ifndef MDRV_H_
#define MDRV_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "generic/nk_error.h"

enum mdrv__state
{
    MDRV__STATE__IDLE,
    MDRV__STATE__PRE_TX,
    MDRV__STATE__PRE_TX_COUNT,
    MDRV__STATE__INIT_TX,
    MDRV__STATE__TX_FH,         /* TX First Half */
    MDRV__STATE__TX_FHI,        /* TX First Half Idle */
    MDRV__STATE__TX_SH,         /* TX Second Half */
    MDRV__STATE__TX_SHI,        /* TX Second Half Idle */
    MDRV__STATE__TX_COMPLETE,
    MDRV__STATE__PRE_RX,
    MDRV__STATE__RX_FHI,
    MDRV__STATE__RX_FHS,
    MDRV__STATE__RX_SHI,
    MDRV__STATE__RX_SHS
};

typedef uint32_t mdrv__time_us_t;

typedef void (pin_init_input_fn)(void*);
typedef void (pin_init_output_fn)(void*, bool);
typedef void (pin_init_trigger_fn)(void*);
typedef void (pin_write_fn)(void*, bool);
typedef bool (pin_read_fn)(void*);
typedef void (tim_start_fn)(void*, mdrv__time_us_t);
typedef void (tim_stop_fn)(void*);
typedef void (tim_start_on_trigger_fn)(void *, mdrv__time_us_t);

struct mdrv__ll
{
    pin_init_input_fn *pin_init_input;
    pin_init_output_fn *pin_init_output;
    pin_init_trigger_fn *pin_init_trigger;
    pin_write_fn *pin_write;
    pin_read_fn *pin_read;
    tim_start_fn *tim_start;
    tim_stop_fn *tim_stop;
    tim_start_on_trigger_fn * tim_start_on_trigger;
};

/* Forward declarations of internal structures */
struct mdrv__config;
struct nk_types__array__u8;

struct mdrv__context
{
    volatile enum mdrv__state p__state;
    size_t p__tx_index;
    size_t p__rx_size;
    uint32_t p__period_count;
    struct mdrv__ll p__ll;
    void *p__ll_context;
    const struct mdrv__config *p__config;
};

enum mdrv__config__idle
{
    MDRV__CONFIG__IDLE__HIGH,
    MDRV__CONFIG__IDLE__LOW,
    MDRV__CONFIG__IDLE__FLOAT
};

enum mdrv__config__pre_tx_state
{
    MDRV__CONFIG__PRE_TX__HIGH,
    MDRV__CONFIG__PRE_TX__LOW,
    MDRV__CONFIG__PRE_TX__FLOAT
};

struct mdrv__config
{
    enum mdrv__config__idle idle;
    enum mdrv__config__pre_tx_state pre_tx_state;
    uint32_t pre_tx_period_count;
    mdrv__time_us_t quarter_period_us;
};

void mdrv__init(struct mdrv__context *context, const struct mdrv__ll *ll, void *ll_context);
int mdrv__set_config(struct mdrv__context *context, const struct mdrv__config *config);
size_t mdrv__get_io_buffer_length(const struct mdrv__context *context);
enum nk_error mdrv__xchg(struct mdrv__context *context,
                         const struct nk_types__array__u8 *wr_data,
                         struct nk_types__array__u8 *rd_data,
                         size_t wr_size,
                         size_t rx_size);
void mdrv__it(struct mdrv__context *context);

#endif /* MDRV_H_ */
