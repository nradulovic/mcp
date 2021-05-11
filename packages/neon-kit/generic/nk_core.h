/*
 * nk_core.h
 *
 *  Created on: May 8, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *  08/05/2021: (nbr) Initial CORE interface definition
 */
/**
 * @file
 * @brief   Neon Kit Core (PSP)
 * @{
 */

#ifndef NEON_KIT_GENERIC_NK_CORE_H_
#define NEON_KIT_GENERIC_NK_CORE_H_

#if defined(__cplusplus)
extern "C"
{
#endif

struct nk_tasker__array;

struct nk_tasker__array * nk_core__tasker_instances(void);

#if defined(__cplusplus)
}
#endif

/**
 * @}
 */
#endif /* NEON_KIT_GENERIC_NK_CORE_H_ */
