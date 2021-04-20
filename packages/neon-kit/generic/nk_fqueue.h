/*
 * nk_fqueue.h
 *
 *  Created on: Apr 5, 2021
 *      Author: nenad
 */

#ifndef NEON_KIT_GENERIC_NK_FQUEUE_H_
#define NEON_KIT_GENERIC_NK_FQUEUE_H_

#include <stddef.h>
#include "nk_array.h"

#define NK_ENABLED_FQUEUE

#define NK_FQUEUE__T(item_type)                                             \
        {                                                                   \
            struct nk_fqueue__p__base fqueue_base;                          \
            struct NK_ARRAY__T(item_type) item_array;                       \
        }

#define NK_FQUEUE__INITIALIZER(item_buffer_p)                               \
        {                                                                   \
            .fqueue_base =                                                  \
            {                                                               \
                .head = 0u,                                                 \
                .tail = 0u,                                                 \
                .size = sizeof(item_buffer_p) / sizeof(item_buffer_p[0]),   \
                .mask = sizeof(item_buffer_p) /                             \
                        sizeof(item_buffer_p[0]) - 1u                       \
            },                                                              \
            NK_ARRAY__INITIALIZER_EMPTY(item_buffer_p)                      \
        }

#define NK_FQUEUE__INITIALIZE(fqueue_p, item_no, item_buffer_p)             \
        do {                                                                \
            nk_fqueue__p__initialize(&(fqueue_p)->fqueue_base, item_no);    \
            NK_ARRAY__INITIALIZE_EMPTY(&(fqueue_p)->item_array,                     \
                 item_no, (item_buffer_p));                                 \
        } while (0)

#define nk_fqueue__item_size(fqueue_p)                                      \
        nk_array__item_size(&(fqueue_p)->item_array)

#define nk_fqueue__item_no(fqueue_p)                                        \
        nk_fqueue__p__size(&(fqueue_p)->fqueue_base)

#define NK_FQUEUE__PUT(fqueue_p, item)                                      \
        do {                                                                \
            (fqueue_p)->item_array.items[                                   \
                nk_fqueue__p__put(&(fqueue_p)->fqueue_base)] = (item);      \
        } while (0)

#define NK_FQUEUE__GET(fqueue_p)                                            \
        (fqueue_p)->item_array.items[                                       \
                nk_fqueue__p__get(&(fqueue_p)->fqueue_base)]

struct nk_fqueue__p__base
{
    size_t head;
    size_t tail;
    size_t size;
    size_t mask;
};

static inline void
nk_fqueue__p__initialize(struct nk_fqueue__p__base *fqueue_base, size_t size)
{
    fqueue_base->head = 0u;
    fqueue_base->tail = 0u;
    fqueue_base->size = size;
    fqueue_base->mask = size - 1u;
}

static inline size_t
nk_fqueue__p__size(const struct nk_fqueue__p__base * fqueue_base)
{
    return fqueue_base->size;
}

static inline size_t
nk_fqueue__p__put(struct nk_fqueue__p__base * fqueue_base)
{
    return fqueue_base->head;
}

static inline size_t
nk_fqueue__p__get(struct nk_fqueue__p__base * fqueue_base)
{
    return fqueue_base->tail;
}

#endif /* NEON_KIT_GENERIC_NK_FQUEUE_H_ */
