/*
 * nk_string.c
 *
 *  Created on: Apr 11, 2021
 *      Author: (nbr) nenad.b.radulovic@gmail.com
 *  11/05/2021: (nbr) Added is_string_literal, append_literal and replace
 */
#include <ctype.h>
#include <string.h>
#include <stdint.h>

#include "generic/nk_bits.h"
#include "generic/nk_string.h"

bool
nk_string__is_equal(const struct nk_string *self, const struct nk_string *other)
{
    if (self->length != other->length) {
        return false;
    }
    for (size_t i = 0u; i < self->length; i++) {
        if (self->items[i] != other->items[i]) {
            return false;
        }
    }
    return true;
}

bool
nk_string__is_equal_literal(const struct nk_string *self, const char *literal, size_t literal_length)
{
    if (self->length != literal_length) {
        return false;
    }
    for (size_t i = 0u; i < self->length; i++) {
        if (self->items[i] != literal[i]) {
            return false;
        }
    }
    return true;
}

struct nk_string
nk_string__view(const struct nk_string *self, size_t from, size_t to)
{
    struct nk_string view;

    NK_ARRAY__INITIALIZE_WINDOW(&view, self, from, to);
    return view;
}

struct nk_string__find__result
nk_string__find(const struct nk_string *self, const struct nk_string *other, size_t from, size_t to)
{
    struct nk_string__find__result result = { .error = NK_ERROR__NOT_FOUND, .value = 0u };

    const struct nk_string source = nk_string__view(self, from, to);
    if (other->length < source.length) {
        for (size_t i = 0u; i <= source.length - other->length; i++) {
            const struct nk_string view = nk_string__view(&source, i, i + other->length);

            if (nk_string__is_equal(&view, other)) {
                result = (struct nk_string__find__result ) { .error = NK_ERROR__OK, .value = i + from};
                break;
            }
        }
    }
    return result;
}

bool
nk_string__contains(const struct nk_string *self, const struct nk_string *other)
{
    struct nk_string__find__result find = nk_string__find(self, other, 0u, SIZE_MAX);

    if (find.error == NK_ERROR__OK) {
        return true;
    } else {
        return false;
    }
}

bool
nk_string__startswith(const struct nk_string *self, const struct nk_string *other)
{
    if (other->length > self->length) {
        return false;
    }
    const struct nk_string view = nk_string__view(self, 0, other->length);
    if (nk_string__is_equal(&view, other)) {
        return true;
    }
    return false;
}

bool
nk_string__endswith(const struct nk_string *self, const struct nk_string *other)
{
    if (other->length > self->length) {
        return false;
    }
    const struct nk_string view = nk_string__view(self, self->length - other->length, self->length);
    if (nk_string__is_equal(&view, other)) {
        return true;
    }
    return false;
}

void
nk_string__clear_all(struct nk_string *self)
{
    self->length = 0u;
}

void
nk_string__rstrip(struct nk_string *self, const struct nk_string *other)
{
    if (nk_string__endswith(self, other)) {
        self->length -= other->length;
    }
}

void
nk_string__lstrip(struct nk_string *self, const struct nk_string *other)
{
    if (nk_string__startswith(self, other)) {
        memmove(self->items, self->items + other->length, self->length - other->length);
        self->length -= other->length;
    }
}

void
nk_string__lower(struct nk_string *self)
{
    for (size_t i = 0u; i < self->length; i++) {
        self->items[i] = (char) tolower(self->items[i]);
    }
}

void
nk_string__upper(struct nk_string *self)
{
    for (size_t i = 0u; i < self->length; i++) {
        self->items[i] = (char) toupper(self->items[i]);
    }
}

void
nk_string__append(struct nk_string *self, const struct nk_string *other)
{
    size_t to_copy = MIN(nk_string__free(self), other->length);

    memcpy(self->items + self->length, other->items, to_copy);
    self->length += to_copy;
}

void
nk_string__append_literal(struct nk_string *self, const char *literal, size_t literal_length)
{
    size_t to_copy = MIN(nk_string__free(self), literal_length);

    memcpy(self->items + self->length, literal, to_copy);
    self->length += to_copy;
}

void
nk_string__append_buffer(struct nk_string *self, const char *buffer)
{
    size_t buffer_len = strlen(buffer);
    size_t to_copy = MIN(nk_string__free(self), buffer_len);

    memcpy(self->items + self->length, buffer, to_copy);
    self->length += to_copy;
}

void
nk_string__copy(struct nk_string *self, const struct nk_string *other)
{
    nk_string__clear_all(self);
    nk_string__append(self, other);
}

void
nk_string__replace(struct nk_string *self, const struct nk_string *search, const struct nk_string *with)
{
    struct nk_string__find__result find;

    if ((self->length == 0u) || (search->length == 0u) || (with->length == 0u)) {
        return;
    }
    if (nk_string__is_equal(search, with)) {
        return;
    }

    find = nk_string__find(self, search, 0, SIZE_MAX);

    if (search->length == with->length) {
        while (find.error == NK_ERROR__OK) {
            memcpy(&self->items[find.value], with->items, with->length);
            find = nk_string__find(self, search, 0, SIZE_MAX);
        }
    } else if (search->length > with->length) {
        while (find.error == NK_ERROR__OK) {
            memmove(&self->items[find.value + with->length],
                    &self->items[find.value + search->length],
                    self->length - (find.value + search->length));
            memcpy(&self->items[find.value], with->items, with->length);
            self->length -= search->length - with->length;
            find = nk_string__find(self, search, find.value + with->length, SIZE_MAX);
        }
    } else {
        while (find.error == NK_ERROR__OK) {
            if (self->item_no > (find.value + with->length)) {
                size_t move_size = self->length - (find.value + search->length);
                move_size = MIN(self->item_no - (find.value + with->length), move_size);
                memmove(&self->items[find.value + search->length],
                        &self->items[find.value + with->length],
                        move_size);
            }
            size_t copy_size = MIN(with->length, self->item_no - find.value);
            memcpy(&self->items[find.value], with->items, with->length);
            self->length += copy_size;
            find = nk_string__find(self, search, 0, SIZE_MAX);
        }
    }
}

char
nk_char__lower(char character)
{
    return (char) tolower(character);
}

char
nk_char__upper(char character)
{
    return (char) toupper(character);
}
