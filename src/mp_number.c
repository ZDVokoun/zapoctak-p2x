#include "mp_number.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int b64_init(struct Base2_64Int *bn, size_t initial_cap) {
  if (bn == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to b64_init\n");
    return -1;
  }
  bn->limbs = calloc(initial_cap, sizeof(uint64_t));
  if (bn->limbs == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in b64_init\n");
    return -1;
  }
  bn->len = 0;
  bn->capacity = initial_cap;
  return 0;
}

void b64_empty(struct Base2_64Int *bn) {
  if (bn == NULL) {
    return;
  }
  for (size_t i = 0; i < bn->capacity; i++)
    bn->limbs[i] = 0;
  bn->len = 0;
}

void b64_free(struct Base2_64Int *bn) {
  if (bn == NULL) {
    return;
  }
  free(bn->limbs);
  bn->limbs = NULL;
  bn->len = 0;
  bn->capacity = 0;
}

int b64_copy(struct Base2_64Int *dst, const struct Base2_64Int *src) {
  if (dst == NULL || src == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to b64_copy\n");
    return -1;
  }
  dst->limbs = malloc(src->capacity * sizeof(uint64_t));
  if (dst->limbs == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in b64_copy\n");
    return -1;
  }
  memcpy(dst->limbs, src->limbs, src->len * sizeof(uint64_t));
  dst->len = src->len;
  dst->capacity = src->capacity;
  return 0;
}

int b64_expand(struct Base2_64Int *bn, size_t new_cap) {
  if (bn == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to b64_expand\n");
    return -1;
  }
  if (new_cap <= bn->capacity) {
    return 0; // No need to expand
  }
  uint64_t *new_limbs = realloc(bn->limbs, new_cap * sizeof(uint64_t));
  if (new_limbs == NULL) {
    fprintf(stderr, "Error: Memory reallocation failed in b64_expand\n");
    return -1;
  }
  memset(new_limbs + bn->capacity, 0,
         (new_cap - bn->capacity) * sizeof(uint64_t));
  bn->limbs = new_limbs;
  bn->capacity = new_cap;
  return 0;
}

void print_base2_64(const struct Base2_64Int *bn) {
  if (bn == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to print_base2_64\n");
    return;
  }
  printf("Base 2^64 representation:\n");
  printf("Length: %zu, Capacity: %zu\n", bn->len, bn->capacity);
  for (size_t i = 0; i < bn->len; i++) {
    printf("Limb %zu: %lu\n", i, bn->limbs[i]);
  }
}

int b64_mul(struct Base2_64Int *bn, uint64_t multiplier, uint64_t addend) {
  uint128_t carry = (uint128_t)addend;

  for (size_t i = 0; i < bn->len; i++) {
    // Early exit condition when doing addition only
    if (carry == 0 && multiplier == 1)
      break;

    uint128_t product = (uint128_t)bn->limbs[i] * (uint128_t)multiplier + carry;

    bn->limbs[i] = (uint64_t)product;

    bn->len = (i + 1 > bn->len) ? i + 1 : bn->len;

    carry = product >> 64;
  }

  if (carry > 0) {
    if (bn->len >= bn->capacity && b64_expand(bn, bn->capacity * 2) != 0) {
      return -1;
    }

    bn->limbs[bn->len] = (uint64_t)carry;
    bn->len++;
  }

  return 0;
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

int base2_64_divmod(struct Base2_64Int *bn, uint64_t divisor,
                    uint64_t *remainder) {
  if (bn == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to base2_64_divmod\n");
    return -1;
  }
  if (divisor == 0) {
    fprintf(stderr, "Error: Division by zero in base2_64_divmod\n");
    return -1;
  }

  uint64_t rem = 0;

  for (size_t i = bn->len; i-- > 0;) {
    uint64_t ql = fast_div128_64(rem, bn->limbs[i], divisor, &rem);
    bn->limbs[i] = ql;
  }

  if (remainder != NULL)
    *remainder = rem;

  if (bn->len >= 1 && bn->limbs[bn->len - 1] == 0)
    bn->len--;

  return 0;
}
