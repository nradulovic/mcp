/*
 * hexador.h
 *
 *  Created on: Mar 6, 2021
 *      Author: nenad
 */

#ifndef HEXADOR_H_
#define HEXADOR_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @pre The size of destination must at least have half-the-size of source
 * @pre Source and destionantion must be non-null pointers
 */
size_t hexador__to_bin(const char * source, uint8_t * destination);
size_t hexador__to_hex(const uint8_t * source, size_t source_size, char * destination);

#endif /* HEXADOR_H_ */
