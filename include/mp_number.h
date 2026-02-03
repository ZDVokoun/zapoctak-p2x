#ifndef ZAPOCTAK_MP_NUMBER_H
#define ZAPOCTAK_MP_NUMBER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"

/**
 * @brief Multi-precision number representation in Base 2^64
 *
 * Limbs are stored in little-endian order.
 */
struct Base2_64Int {
  uint64_t *limbs;
  size_t len;
  size_t capacity;
};

/**
 * @brief Initialize a `Base2_64Int` number with given capacity
 *
 * @param bn Output number to initialize
 * @param initial_cap Initial capacity in limbs
 * @return 0 on success
 * @retval -1 if memory allocation fails
 */
int b64_init(struct Base2_64Int *bn, size_t initial_cap);

/**
 * @brief Free resources allocated for a `Base2_64Int` number
 *
 * @param bn Number to free
 */
void b64_free(struct Base2_64Int *bn);

/**
 * @brief Print `Base2_64Int` representation (debug)
 *
 * @param bn Number to print
 */
void print_base2_64(const struct Base2_64Int *bn);

/**
 * @brief Copy a `Base2_64Int` number
 *
 * @param dst Destination number
 * @param src Source number
 * @return 0 on success
 * @retval -1 if memory allocation fails
 */
int b64_copy(struct Base2_64Int *dst, const struct Base2_64Int *src);

/**
 * @brief Expand the capacity of a `Base2_64Int` number
 *
 * @param bn Number to expand
 * @param new_cap New capacity in limbs
 * @return 0 on success
 * @retval -1 if memory allocation fails
 */
int b64_expand(struct Base2_64Int *bn, size_t new_cap);

/**
 * @brief Multiply `Base2_64Int` by a scalar (64-bit unsigned integer) and add an addend
 *
 * Performs: bn = bn * multiplier + addend
 *
 * @param bn Number to update
 * @param multiplier Scalar multiplier
 * @param addend Scalar addend
 * @return 0 on success
 * @retval -1 if memory allocation fails or if negative numbers are used
 */
int b64_mul(struct Base2_64Int *bn, uint64_t multiplier, uint64_t addend);

/**
 * @brief Division with modulo for `Base2_64Int` by a 64-bit divisor
 *
 * Result: bn = bn / divisor, *remainder = bn % divisor
 *
 * @param bn Number to divide (updated in-place)
 * @param divisor 64-bit divisor
 * @param remainder Output remainder
 * @return 0 on success
 * @retval -1 if divisor == 0
 */
int base2_64_divmod(struct Base2_64Int *bn, uint64_t divisor,
                     uint64_t *remainder);

/**
 * @brief Fast division of 128-bit number by 64-bit divisor
 *
 * Uses x86-64 `divq` instruction.
 *
 * @param high High 64 bits of the dividend
 * @param low Low 64 bits of the dividend
 * @param divisor 64-bit divisor
 * @param rem Output remainder
 * @return Quotient of the division
 * @pre high < divisor
 */
uint64_t fast_div128_64(uint64_t high, uint64_t low, uint64_t divisor,
                        uint64_t *rem);

#endif // ZAPOCTAK_MP_NUMBER_H
