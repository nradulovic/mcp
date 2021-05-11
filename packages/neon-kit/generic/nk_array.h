/*
 * nk_array.h
 *
 *  Created on: Apr 5, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *
 *  08/05/2021: (nbr) Minor code formatting
 *  11/05/2021: (nbr) Empty initializer will not initialize the buffer
 */

#ifndef NEON_KIT_GENERIC_NK_ARRAY_H_
#define NEON_KIT_GENERIC_NK_ARRAY_H_

#include <stddef.h>
#include <string.h>

#include "nk_bits.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define NK_ENABLED_ARRAY

/**
 * @brief   Array construction
 *
 * There are multiple ways to construct an array. The following methods of array construction are supported:
 * 1. Static construction of an array with static buffer, @ref NK_ARRAY__INITIALIZER
 * 2. Static construction of empty array with static buffer, @ref NK_ARRAY__INITIALIZER_EMPTY
 * 3. Runtime construction of an array using dynamic buffer, @ref NK_ARRAY__INITIALIZE
 * 4. Runtime construction of empty array using dynamic buffer, @ref NK_ARRAY__INITIALIZE_EMPTY
 * 5. Runtime construction of an array with static buffer, @ref NK_ARRAY__INITIALIZE_WITH
 * 6. Runtime construction of empty array with static buffer, @ref NK_ARRAY__INITIALIZE_EMPTY_WITH
 */

/**
 * @brief   Array bucket construction
 *
 * The following methods of array bucket construction are supported:
 * 1. Static construction of an array bucket with static initializer, @ref NK_ARRAY__BUCKET_INITIALIZER
 * 2. Static construction of empty array bucket, @ref NK_ARRAY__BUCKET_INITIALIZER_EMPTY
 * 3. Runtime construction of an array bucket from dynamic buffer, @ref NK_ARRAY__BUCKET_INITIALIZE
 * 4. Runtime construction of empty array bucket, @ref NK_ARRAY__BUCKET_INITIALIZE_EMPTY
 * 5. Runtime construction of non-empty array with static buffer, @ref NK_ARRAY__BUCKET_INITIALIZE_WITH
 */

#define NK_ARRAY__T(item_type)                                              \
        {                                                                   \
            size_t item_no; /* Number of items in buffer */                 \
            size_t length;                                                  \
            item_type * items;                                              \
        }

/**
 * @brief   Static construction of array with static buffer
 *
 * @code
 * item_t buffer[N] = { 1, 2, 3, 4 };
 * struct my_array_type NK_ARRAY__T(item_t) my_array = NK_ARRAY__INITIALIZER(buffer, 4);
 * @endcode
 */
#define NK_ARRAY__INITIALIZER(static_buffer, buffer_length)                 \
        {                                                                   \
            .item_no = NK_BITS__ARRAY_SIZE(static_buffer),                  \
            .length = (buffer_length),                                      \
            .items = static_buffer                                          \
        }

/**
 * @brief   Static construction of empty array with static buffer
 *
 * @code
 * item_t buffer[N] = { 0 };
 * struct my_array_type NK_ARRAY__T(item_t) my_array = NK_ARRAY__INITIALIZER_EMPTY(buffer);
 * @endcode
 */
#define NK_ARRAY__INITIALIZER_EMPTY(static_buffer)                          \
        NK_ARRAY__INITIALIZER((static_buffer), 0u)

/**
 * @brief   Runtime construction of an array using dynamic buffer
 *
 * @code
 * item_t * buffer;
 * size_t buffer_size;
 * size_t occupied;
 * struct my_array_type NK_ARRAY__T(item_t) my_array;
 *
 * NK_ARRAY__INITIALIZE(&my_array, buffer_size, buffer, occupied);
 * @endcode
 */
#define NK_ARRAY__INITIALIZE(array_p, buffer_size, buffer_p, buffer_length) \
        do {                                                                \
            (array_p)->item_no = (buffer_size);                             \
            (array_p)->items = (buffer_p);                                  \
            (array_p)->length = (buffer_length);                            \
        } while (0)

/**
 * @brief   Runtime construction of empty array using dynamic buffer
 *
 * @code
 * item_t * buffer;
 * size_t buffer_size;
 * struct my_array_type NK_ARRAY__T(item_t) my_array;
 *
 * NK_ARRAY__INITIALIZE_EMPTY(&my_array, buffer_size, buffer);
 * @endcode
 */
#define NK_ARRAY__INITIALIZE_EMPTY(array_p, buffer_size, buffer_p)          \
        NK_ARRAY__INITIALIZE((array_p), (buffer_size), (buffer_p), 0u)

/**
 * @brief   Runtime construction of non-empty array with static buffer
 */
#define NK_ARRAY__INITIALIZE_WITH(array_p, static_buffer, buffer_length)    \
        NK_ARRAY__INITIALIZE(                                               \
                (array_p),                                                  \
                NK_BITS__ARRAY_SIZE(static_buffer),                         \
                static_buffer,                                              \
                (buffer_length))

/**
 * @brief   Runtime construction of empty array with static buffer
 */
#define NK_ARRAY__INITIALIZE_EMPTY_WITH(array_p, static_buffer)             \
        NK_ARRAY__INITIALIZE_WITH(array_p, static_buffer, 0u)


#define NK_ARRAY__INITIALIZE_WINDOW(window_p, array_p, from, to)            \
        do {                                                                \
            size_t l_from = (from);                                         \
            size_t l_to = (to);                                             \
            size_t view_from = MIN(l_from, (array_p)->length);              \
            size_t view_to = MIN(l_to, (array_p)->length);                  \
            size_t length = view_to - view_from;                            \
            NK_ARRAY__INITIALIZE(                                           \
                    (window_p),                                             \
                    length,                                                 \
                    &(array_p)->items[view_from],                           \
                    length);                                                \
        } while (0)

#define NK_ARRAY__BUCKET_T(item_type, item_no)                              \
        {                                                                   \
            struct NK_ARRAY__T(item_type) array;                            \
            item_type buffer[item_no];                                      \
        }

#define NK_ARRAY__BUCKET_TYPED_T(item_type, item_no, array_type)            \
        {                                                                   \
            array_type array;                                               \
            item_type buffer[item_no];                                      \
        }

/**
 * @brief   Static construction of non-empty array bucket with static initializer
 */
#define NK_ARRAY__BUCKET_INITIALIZER(self, a_length, ...)                   \
        {                                                                   \
            .array = NK_ARRAY__INITIALIZER((self)->buffer, (a_length)),     \
            .buffer = __VA_ARGS__                                           \
        }

/**
 * @brief   Static construction of empty array bucket
 */
#define NK_ARRAY__BUCKET_INITIALIZER_EMPTY(self)                            \
        {                                                                   \
            .array = NK_ARRAY__INITIALIZER((self)->buffer, 0)               \
        }

/**
 * @brief   Runtime construction of an array bucket from dynamic buffer
 */
#define NK_ARRAY__BUCKET_INITIALIZE(array_p, buffer_p, buffer_length)       \
        do {                                                                \
            size_t l_buffer_size = NK_BITS__ARRAY_SIZE((array_p)->buffer);  \
            size_t l_buffer_length = MIN(l_buffer_size, buffer_length);     \
            void * l_buffer = (void *)(buffer_p);                           \
            NK_ARRAY__INITIALIZE(                                           \
                    &((array_p)->array),                                    \
                    l_buffer_size,                                          \
                    (array_p)->buffer,                                      \
                    l_buffer_length);                                       \
            if (l_buffer != NULL) {                                         \
                memcpy((array_p)->buffer, l_buffer, l_buffer_length);       \
            }                                                               \
        } while (0)

/**
 * @brief   Runtime construction of empty array bucket
 */
#define NK_ARRAY__BUCKET_INITIALIZE_EMPTY(array_p)                          \
        NK_ARRAY__INITIALIZE_EMPTY(                                         \
                &((array_p)->array),                                        \
                NK_BITS__ARRAY_SIZE((array_p)->buffer),                     \
                (array_p)->buffer)

/**
 * @brief   Runtime construction of non-empty array with static buffer
 */
#define NK_ARRAY__BUCKET_INITIALIZE_WITH(array_p, static_buffer)            \
        NK_ARRAY__BUCKET_INITIALIZE(                                        \
                (array_p),                                                  \
                static_buffer,                                              \
                NK_BITS__ARRAY_SIZE(static_buffer))


#define NK_ARRAY__ITEM_SIZE(array_p)                                        \
        sizeof(*(array_p)->items)

#define NK_ARRAY__FREE(array_p)                                             \
        ((array_p)->item_no - (array_p)->length)

#if defined(__cplusplus)
}
#endif

#endif /* NEON_KIT_GENERIC_NK_ARRAY_H_ */
