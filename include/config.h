#ifndef ZAPOCTAK_CONFIG_H
#define ZAPOCTAK_CONFIG_H

#include <stddef.h>
#include <stdint.h>

#if __BITINT_MAXWIDTH__ < 128
#error "This C23 compiler does not support 128-bit integers needed for fast Base 2^64 arithmetic."
#endif

typedef unsigned _BitInt(128) uint128_t;

// Mersenne moduli used in residue number system
extern size_t moduli64len;
extern uint64_t moduli64[15];

#endif // ZAPOCTAK_CONFIG_H
