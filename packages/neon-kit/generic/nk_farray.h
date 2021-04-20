/*
 * nk_farray.h
 *
 *  Created on: Apr 13, 2021
 *      Author: nenad
 */

#ifndef GENERIC_NK_FARRAY_H_
#define GENERIC_NK_FARRAY_H_

#define NK_FARRAY__T(item_type, item_no)                                    \
		{                                                                   \
            item_type items[item_no];                                       \
		}

#define NK_FARRAY__LENGTH(self)                                             \
        (sizeof((self)->items) / sizeof((self)->items[0]))

#endif /* GENERIC_NK_FARRAY_H_ */
