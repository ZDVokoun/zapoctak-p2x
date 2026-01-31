#include "config.h"
#include "mp_number.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void b64_init(struct Base2_64Int *bn, size_t initial_cap) {
  bn->limbs = calloc(initial_cap, sizeof(uint64_t));
  bn->len = 0;
  bn->capacity = initial_cap;
}

void b64_free(struct Base2_64Int *bn) {
  free(bn->limbs);
  bn->len = 0;
  bn->capacity = 0;
}

void print_base2_64(const struct Base2_64Int *bn) {
  printf("Base 2^64 representation:\n");
  for (size_t i = 0; i < bn->len; i++) {
    printf("Limb %zu: %lu\n", i, bn->limbs[i]);
  }
}

void b64_mul(struct Base2_64Int *bn, uint64_t multiplier, uint64_t addend) {
  uint128_t carry = (uint128_t)addend;

  for (size_t i = 0; i < bn->capacity; i++) {
    // Early exit conditions
    if (carry == 0 && bn->limbs[i] == 0)
      break;

    // Early exit condition when doing addition only
    if (carry == 0 && multiplier == 1)
      break;

    uint128_t product = (uint128_t)bn->limbs[i] * (uint128_t)multiplier + carry;

    bn->limbs[i] = (uint64_t)product;

    bn->len = (i + 1 > bn->len) ? i + 1 : bn->len;

    carry = product >> 64;
  }

  if (carry > 0) {
    if (bn->len >= bn->capacity) {
      size_t old_capacity = bn->capacity;
      bn->capacity *= 2;
      bn->limbs = realloc(bn->limbs, bn->capacity * sizeof(uint64_t));

      if (bn->limbs == NULL) {
        // TODO Throw error
        return;
      }

      memset(bn->limbs + old_capacity, 0, old_capacity * sizeof(uint64_t));
    }
    bn->limbs[bn->len] = (uint64_t)carry;
    bn->len++;
  }
}

uint64_t fast_div128_64(uint64_t high, uint64_t low, uint64_t divisor,
                        uint64_t *rem) {
  // Optimized for x86-64: Uses single 'divq' instruction
  // PRECONDITION: high < divisor
  uint64_t quotient;

  __asm__("divq %[v]"
          : "=a"(quotient),
            "=d"(*rem) // Outputs: a=RAX (quotient), d=RDX (remainder)
          : "d"(high), "a"(low), [v] "r"(divisor) // Inputs: RDX=high, RAX=low
          : "cc"                                  // Clobbers flags
  );

  return quotient;
}

void base2_64_divmod(struct Base2_64Int *bn, uint64_t divisor,
                     uint64_t *remainder) {
  uint64_t rem = 0;

  for (size_t i = bn->len; i-- > 0;) {
    uint64_t ql = fast_div128_64(rem, bn->limbs[i], divisor, &rem);
    bn->limbs[i] = ql;
  }

  *remainder = rem;

  if (bn->len >= 1 && bn->limbs[bn->len - 1] == 0)
    bn->len--;
}
