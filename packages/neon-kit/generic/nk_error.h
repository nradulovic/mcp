/*
 * nk_error.h
 *
 *  Created on: Apr 11, 2021
 *      Author: nenad
 */

#ifndef NEON_KIT_GENERIC_NK_ERROR_H_
#define NEON_KIT_GENERIC_NK_ERROR_H_

enum nk_error
{
    NK_ERROR__OK,
    /* Query and search errors */
    NK_ERROR__NOT_FOUND,

    /* Data error */
    NK_ERROR__DATA_INVALID,
    NK_ERROR__DATA_ODD,
    NK_ERROR__DATA_OVF,
    NK_ERROR__DATA_UNDERFLOW,

    /* Buffer error */
    NK_ERROR__BUFFER_OVF
};

#endif /* NEON_KIT_GENERIC_NK_ERROR_H_ */
