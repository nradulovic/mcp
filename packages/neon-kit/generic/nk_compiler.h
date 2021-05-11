/*
 * nk_compiler.h
 *
 *  Created on: May 8, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 */
/**
 * @file
 * @brief   Neon Kit Compiler (PLATFORM)
 * @{
 */

#ifndef NEON_KIT_GENERIC_NK_COMPILER_H_
#define NEON_KIT_GENERIC_NK_COMPILER_H_

#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif

uint32_t nk_compiler__instruction_count(void);

#if defined(__cplusplus)
}
#endif

/**
 * @}
 */
#endif /* NEON_KIT_GENERIC_NK_COMPILER_H_ */
