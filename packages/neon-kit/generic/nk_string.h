/*
 * nk_string.h
 *
 *  Created on: Apr 10, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *
 *  08/05/2021: (nbr) Minor code formatting
 *  11/05/2021: (nbr) Added is_string_literal, append_literal and replace
 */

#ifndef NEON_KIT_GENERIC_NK_STRING_H_
#define NEON_KIT_GENERIC_NK_STRING_H_

#include <stddef.h>
#include <stdbool.h>

#include "nk_array.h"
#include "nk_result.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define NK_ENABLED_STRING

/**
 * @brief   String construction
 *
 * There are multiple ways to construct a string. The following methods of string construction are supported:
 * 1. Static construction of empty string using static buffer
 * 2. Static construction of non-empty string using static buffer
 * 3. Runtime construction of empty string using dynamic buffer
 * 4. Runtime construction of non-empty string using dynamic buffer
 * 5. Runtime construction of non-empty string from static buffer
 */

/**
 * @brief   String bucket construction
 * 1. Empty bucket
 * 2. Non-empty bucket with string initializer
 * 3. Non-empty bucket with dynamic string
 */

struct nk_string NK_ARRAY__T(char);

/**
 * @brief   Static construction of non-empty string using static buffer
 */
#define NK_STRING__INITIALIZER(static_buffer, static_buffer_length)         \
        NK_ARRAY__INITIALIZER(static_buffer, (static_buffer_length))

/**
 * @brief   Static construction of empty string using static buffer
 */
#define NK_STRING__INITIALIZER_EMPTY(static_buffer)                         \
        NK_ARRAY__INITIALIZER(static_buffer, 0u)

#define NK_STRING__BUCKET_T(char_no)                                        \
        NK_ARRAY__BUCKET_TYPED_T(char, char_no, struct nk_string)

#define NK_STRING__BUCKET_INITIALIZER(self, static_string)                  \
        NK_ARRAY__BUCKET_INITIALIZER((self), sizeof(static_string) - 1u, static_string)

#define NK_STRING__BUCKET_INITIALIZER_EMPTY(self)                           \
        NK_ARRAY__BUCKET_INITIALIZER_EMPTY((self))

#define NK_STRING__BUCKET_INITIALIZE(self, string_p, string_length)         \
        NK_ARRAY__BUCKET_INITIALIZE(self, string_p, string_length)

#define NK_STRING__BUCKET_INITIALIZE_EMPTY(self)                            \
        NK_ARRAY__BUCKET_INITIALIZE_EMPTY(self)

#define NK_STRING__BUCKET_INITIALIZE_WITH(self, static_string)              \
        NK_ARRAY__BUCKET_INITIALIZE(self, static_string, sizeof(static_string) - 1u)

#define NK_STRING__LITERAL(static_string)                                   \
        (static_string), (sizeof(static_string) - 1u)

struct nk_string__find__result
    NK_RESULT__T(size_t);

/**
 * @brief   Runtime construction of empty string using dynamic buffer
 */
static inline void
nk_string__initialize_empty(struct nk_string *string, char *string_buffer, size_t string_buffer_size)
{
    NK_ARRAY__INITIALIZE_EMPTY(string, string_buffer_size, string_buffer);
}

/**
 * @brief   Runtime construction of non-empty string using dynamic buffer
 */
static inline void
nk_string__initialize(struct nk_string *string, char *string_buffer, size_t string_buffer_size, size_t string_size)
{
    NK_ARRAY__INITIALIZE(string, string_buffer_size, string_buffer, string_size);
}

static inline size_t
nk_string__length(const struct nk_string *self)
{
    return self->length;
}

static inline size_t
nk_string__free(const struct nk_string * self)
{
    return NK_ARRAY__FREE(self);
}

static inline size_t
nk_string__size(const struct nk_string *self)
{
    return self->item_no;
}

static inline char
nk_string__char(const struct nk_string *self, size_t index)
{
    return self->items[index];
}

bool
nk_string__is_equal(const struct nk_string *self, const struct nk_string *other);

bool
nk_string__is_equal_literal(const struct nk_string * self, const char * literal, size_t literal_length);

struct nk_string
nk_string__view(const struct nk_string *self, size_t from, size_t to);

struct nk_string__find__result
nk_string__find(const struct nk_string *self, const struct nk_string *other, size_t from, size_t to);

bool
nk_string__contains(const struct nk_string *self, const struct nk_string *other);

bool
nk_string__startswith(const struct nk_string *self, const struct nk_string *other);

bool
nk_string__endswith(const struct nk_string *self, const struct nk_string *other);

void
nk_string__clear_all(struct nk_string * self);

/**
 * \brief       Modifies string \a self and removes trailing characters specified in \a other removed
 */
void
nk_string__rstrip(struct nk_string *self, const struct nk_string *other);

void
nk_string__lstrip(struct nk_string *self, const struct nk_string *other);

void
nk_string__lower(struct nk_string * self);

void
nk_string__upper(struct nk_string *self);

void
nk_string__append(struct nk_string *self, const struct nk_string *other);

void
nk_string__append_literal(struct nk_string *self, const char * literal, size_t literal_length);

void
nk_string__append_c_string(struct nk_string *self, const char * literal);

void
nk_string__copy(struct nk_string *self, const struct nk_string * other);

void
nk_string__replace(struct nk_string *self, const struct nk_string *search, const struct nk_string *with);

char nk_char__lower(char character);

char nk_char__upper(char character);

#if defined(__cplusplus)
}
#endif

#endif /* NEON_KIT_GENERIC_NK_STRING_H_ */
