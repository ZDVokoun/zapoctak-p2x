#ifndef ZAPOCTAK_CONFIG_H
#define ZAPOCTAK_CONFIG_H

#include <stddef.h>
#include <stdint.h>

#if __BITINT_MAXWIDTH__ < 128
#error                                                                         \
    "This C23 compiler does not support 128-bit integers needed for fast Base 2^64 arithmetic."
#endif

typedef unsigned _BitInt(128) uint128_t;

/**
 * @brief Number of Mersenne moduli used
 */
extern size_t moduli64len;

/**
 * @brief Array of Mersenne moduli bit sizes
 */
extern uint64_t moduli64[18];

/**
 * @brief Initialize the global moduli array with coprimes starting from n
 *
 * Generates a greedy set of pairwise coprime integers starting from n down to 2.
 * These are used as exponents for Mersenne moduli 2^k - 1.
 *
 * @param n Starting value
 * @pre n >= 2 && n <= 64
 * @note Time complexity: O(n * moduli64len)
 */
void init_moduli_greedy(uint64_t n);

/**
 * @brief Initialize the global moduli array with prime powers up to n
 *
 * For each prime p <= n, finds the greatest power m = p^k such that m <= n.
 *
 * @param n Upper bound for prime powers
 * @pre n >= 2 && n <= 64
 * @note Time complexity: O(n * moduli64len)
 */
void init_moduli_prime_powers(uint64_t n);

#endif // ZAPOCTAK_CONFIG_H
