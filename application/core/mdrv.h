/*
 * mdrv.h
 *
 *  Created on: Mar 3, 2021
 *      Author: nenad
 */

#ifndef MDRV_H_
#define MDRV_H_

#include <stdbool.h>
#include <stdint.h>

enum mdrv__state
{
    MDRV__STATE__IDLE,
    MDRV__STATE__PRE_TX,
    MDRV__STATE__PRE_TX_COUNT,
    MDRV__STATE__INIT_TX,
    MDRV__STATE__TX,
    MDRV__STATE__TX_COMPLETE
};

typedef uint32_t mdrv__time_us_t;

typedef void (pin_init_input_fn)(void*);
typedef void (pin_init_output_fn)(void*, bool);
typedef void (pin_write_fn)(void*, bool);
typedef bool (pin_read_fn)(void*);
typedef void (tim_start_fn)(void*, mdrv__time_us_t);
typedef void (tim_stop_fn)(void*);

struct mdrv__ll
{
    pin_init_input_fn *pin_init_input;
    pin_init_output_fn *pin_init_output;
    pin_write_fn *pin_write;
    pin_read_fn *pin_read;
    tim_start_fn *tim_start;
    tim_stop_fn *tim_stop;
};

struct mdrv__config;

struct mdrv__context
{
    enum mdrv__state p__state;
    const void *p__wr_data;
    size_t p__wr_size;
    size_t p__wr_index;
    uint32_t p__period_count;
    struct mdrv__ll p__ll;
    void *p__ll_context;
    const struct mdrv__config *p__config;
    volatile bool p__wait;
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
    mdrv__time_us_t tx_period;
};

void mdrv__init(struct mdrv__context *context, const struct mdrv__ll *ll, void *ll_context);
int mdrv__set_config(struct mdrv__context *context, const struct mdrv__config *config);
size_t mdrv__get_io_buffer_length(const struct mdrv__context *context);
int mdrv__write(struct mdrv__context *context, const void *data, size_t size);
void mdrv__it(struct mdrv__context *context);

#endif /* MDRV_H_ */
