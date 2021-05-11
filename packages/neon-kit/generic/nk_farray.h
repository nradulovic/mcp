/*
 * nk_farray.h
 *
 *  Created on: Apr 13, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *
 *  08/05/2021: (nbr) Added more macros
 */

#ifndef GENERIC_NK_FARRAY_H_
#define GENERIC_NK_FARRAY_H_

#if defined(__cplusplus)
extern "C"
{
#endif

#include <string.h>

#define NK_FARRAY__T(item_type, item_no)                                    \
		{                                                                   \
            item_type items[item_no];                                       \
		}

#define NK_FARRAY__INITIALIZE_EMPTY(self)                                   \
        memset((self)->items, 0, NK_FARRAY__SIZE(self) * NK_FARRAY__ITEM_SIZE(self))

#define NK_FARRAY__ITEM_SIZE(self)                                          \
        (sizeof((self)->items[0]))

#define NK_FARRAY__LENGTH(self)                                             \
        (sizeof((self)->items) / NK_FARRAY__ITEM_SIZE(self))

#define NK_FARRAY__SIZE(self)                                               \
        NK_FARRAY__LENGTH(self)

#if defined(__cplusplus)
}
#endif

#endif /* GENERIC_NK_FARRAY_H_ */
