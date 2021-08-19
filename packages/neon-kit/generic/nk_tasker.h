/*
 * nk_tasker.h
 *
 *  Created on: May 8, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 */

#ifndef NEON_KIT_GENERIC_NK_TASKER_H_
#define NEON_KIT_GENERIC_NK_TASKER_H_

#include <stdint.h>
#include "generic/nk_farray.h"
#include "generic/nk_array.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define NK_TASKER__MAX_TASKS            32u
#define NK_TASKER__MAX_PRIO             (NK_TASKER__MAX_TASKS - 1u)

struct nk_cpu__context;
struct nk_task;

struct nk_tasker
{
    struct nk_tasker__queue
    {
        uint32_t bitmap;
    } p__ready;
    struct nk_task_array
        NK_FARRAY__T(struct nk_task *, NK_TASKER__MAX_TASKS)
    p__tasks;
    struct nk_task *p__current;
};

struct nk_tasker__array
    NK_ARRAY__T(struct nk_tasker *)
;

/** @brief  Initialize tasker instances
 *
 * Initialize each instance given in instance array
 */
void
nk_tasker__initialize(struct nk_tasker__array *instances);

void
nk_tasker__start(struct nk_tasker *tasker, struct nk_task *task);

void
nk_tasker__yield(struct nk_tasker *tasker);

void
nk_tasker__disable(struct nk_tasker *tasker);

void
nk_tasker__enable(struct nk_tasker *tasker);

static inline struct nk_task *
nk_tasker__current(struct nk_tasker * tasker)
{
    return tasker->p__current;
}

#define NK_TASK__BUCKET_T(stack_size)                                       \
        {                                                                   \
            struct nk_task task;                                            \
            struct NK_ARRAY__BUCKET_TYPED_T(                                \
                    uint8_t, (stack_size), nk_types__array__u8) stack;      \
        }

#define NK_TASK__BUCKET_INITIALIZER(self, method, arg, prio)                \
        {                                                                   \
            .task =                                                         \
            {                                                               \
                .p__arg = (arg),                                            \
                .p__method = (method),                                      \
                .p__prio = (prio),                                          \
                .p__context =                                               \
                {                                                           \
                    .p__stack = &(self)->stack.array                        \
                }                                                           \
            },                                                              \
            .stack = NK_ARRAY__BUCKET_INITIALIZER_EMPTY(&(self)->stack)     \
        }

typedef void (nk_task__method_t)(void *);

struct nk_task
{
    void *p__arg;
    nk_task__method_t *p__method;
    uint_fast32_t p__prio;
    union nk_task__context
    {
        struct nk_cpu__context *p__context;
        struct nk_types__array_u8 *p__stack;
    } p__context;
};

static inline uint32_t
nk_task__get_prio(const struct nk_task *self)
{
    return (uint32_t)self->p__prio;
}

struct nk_completion
{
    struct nk_tasker * p__tasker;
    struct nk_tasker__queue p__blocked;
};

void
nk_completion__initialize(struct nk_completion * self, struct nk_tasker * tasker);

void
nk_completion__signal(struct nk_completion * self);

void
nk_completion__wait(struct nk_completion * self);

struct nk_semaphore
{
    int32_t p__count;
    struct nk_tasker * p__tasker;
    struct nk_tasker__queue p__blocked;
};

void
nk_semaphore__initialize(struct nk_semaphore * self, int32_t count, struct nk_tasker * tasker);

void
nk_semaphore__signal(struct nk_semaphore * self);

void
nk_semaphore__wait(struct nk_semaphore * self);



static inline int32_t
nk_semaphore__get_count(const struct nk_semaphore *self)
{
    return self->p__count;
}

#if defined(__cplusplus)
}
#endif

#endif /* NEON_KIT_GENERIC_NK_TASKER_H_ */
