#ifndef ZAPOCTAK_MP_NUMBER_H
#define ZAPOCTAK_MP_NUMBER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"

/**
 * Multi-precision number representation in Base 2^64
 * Limbs are stored in little-endian order
 */
struct Base2_64Int {
  uint64_t *limbs;
  size_t len;
  size_t capacity;
};

/**
 * Initialize a Base2_64Int number with given capacity
 * Returns: 0 on success, -1 on memory allocation failure
 */
int b64_init(struct Base2_64Int *bn, size_t initial_cap);

/**
 * Free resources allocated for a Base2_64Int number
 */
void b64_free(struct Base2_64Int *bn);

/**
 * Print Base2_64Int representation (debug)
 */
void print_base2_64(const struct Base2_64Int *bn);

/**
 * Copy a Base2_64Int number
 * Returns: 0 on success, -1 on memory allocation failure
 */
int b64_copy(struct Base2_64Int *dst, const struct Base2_64Int *src);

/**
 * Expand the capacity of a Base2_64Int number
 * Returns: 0 on success, -1 on memory allocation failure
 */
int b64_expand(struct Base2_64Int *bn, size_t new_cap);

/**
 * Multiply Base2_64Int by a scalar and add an addend
 * Performs: bn = bn * multiplier + addend
 * Returns: 0 on success, -1 on memory allocation failure or if negative numbers are used
 */
int b64_mul(struct Base2_64Int *bn, uint64_t multiplier, uint64_t addend);

/**
 * Division with modulo for Base2_64Int by a 64-bit divisor
 * Result: bn = bn / divisor, *remainder = bn % divisor
 * Returns: 0 on success, -1 on invalid arguments (divisor == 0)
 */
int base2_64_divmod(struct Base2_64Int *bn, uint64_t divisor,
                     uint64_t *remainder);

/**
 * Fast division of 128-bit number by 64-bit divisor
 * Uses x86-64 'divq' instruction
 * PRECONDITION: high < divisor
 */
uint64_t fast_div128_64(uint64_t high, uint64_t low, uint64_t divisor,
                        uint64_t *rem);

#endif // ZAPOCTAK_MP_NUMBER_H
