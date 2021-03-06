/*
 * mdrv.h
 *
 *  Created on: Mar 3, 2021
 *      Author: nenad
 */

#ifndef MDRV_H_
#define MDRV_H_

#include <stdbool.h>

enum mdrv__state
{
    MDRV__STATE__IDLE,
    MDRV__STATE__WRITE
};

typedef void (pin_init_input_fn)(void *);
typedef void (pin_init_output_fn)(void *);
typedef void (pin_write_fn)(void *, bool);
typedef bool (pin_read_fn)(void *);
typedef void (tim_start_fn)(void *);
typedef void (tim_stop_fn)(void *);

struct mdrv__ll
{
    pin_init_input_fn * pin_init_input;
    pin_init_output_fn * pin_init_output;
    pin_write_fn * pin_write;
    pin_read_fn * pin_read;
    tim_start_fn * tim_start;
    tim_stop_fn * tim_stop;
};

struct mdrv__context
{
    enum mdrv__state p__state;
    const void * p__wr_data;
    size_t p__wr_size;
    size_t p__wr_index;
    struct mdrv__ll p__ll;
    void * p__ll_context;
};

void mdrv__init(struct mdrv__context * context, const struct mdrv__ll * ll, void * ll_context);
void mdrv__write(struct mdrv__context * context, const void * data, size_t size);
void mdrv__it(struct mdrv__context *context);

#endif /* MDRV_H_ */
