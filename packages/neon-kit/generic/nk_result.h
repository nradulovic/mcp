/*
 * nk_result.h
 *
 *  Created on: Apr 11, 2021
 *      Author: nenad
 */

#ifndef NEON_KIT_GENERIC_NK_RESULT_H_
#define NEON_KIT_GENERIC_NK_RESULT_H_

#include "nk_error.h"

#define NK_RESULT__T(value_type)                                            \
        {                                                                   \
            enum nk_error error;                                            \
            value_type value;                                               \
        }

#endif /* NEON_KIT_GENERIC_NK_RESULT_H_ */
