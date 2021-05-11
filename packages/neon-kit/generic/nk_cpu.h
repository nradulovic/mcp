/*
 * nk_kernel.h
 *
 *  Created on: May 8, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *  08/05/2021: (nbr) Initial CPU interface definition
 */
/**
 * @file
 * @brief   Neon Kit CPU (KERNEL)
 * @{
 */

#ifndef NEON_KIT_GENERIC_NK_CPU_H_
#define NEON_KIT_GENERIC_NK_CPU_H_

#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif

struct nk_task;
struct nk_cpu__context;

struct nk_cpu__isr
{
    union nk_cpu__isr__variants
    {
        uint32_t u32;
        void * vp;
    } variant;
};

uint32_t nk_cpu__instruction_count(void);
struct nk_cpu__context * nk_cpu__create_context(struct nk_task * task);
void nk_cpu__isr__disable(struct nk_cpu__isr * isr);
void nk_cpu__isr__enable(struct nk_cpu__isr * isr);
uint32_t nk_cpu__ffs(uint32_t);

#if defined(__cplusplus)
}
#endif

/**
 * @}
 */
#endif /* NEON_KIT_GENERIC_NK_CPU_H_ */
