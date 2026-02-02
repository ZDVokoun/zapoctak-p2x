#include "config.h"
#include "mp_number.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
  bn->sign = 0; // Positive by default
  bn->capacity = initial_cap;
  return 0;
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
  dst->sign = src->sign;
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
  memset(new_limbs + bn->capacity, 0, (new_cap - bn->capacity) * sizeof(uint64_t));
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
  printf("Length: %zu, Capacity: %zu, Sign: %s\n", bn->len, bn->capacity,
         bn->sign ? "Negative" : "Positive");
  for (size_t i = 0; i < bn->len; i++) {
    printf("Limb %zu: %lu\n", i, bn->limbs[i]);
  }
}

int b64_b64_cmp(const struct Base2_64Int *a, const struct Base2_64Int *b) {
  /*
  * Compares absolute values of two Base2_64Int numbers.
  */
  if (a == NULL || b == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to b64_b64_cmp\n");
    return 0;
  }
  
  if (a->len > b->len) return 1;
  if (a->len < b->len) return -1;

  for (size_t i = a->len; i-- > 0;) {
    if (a->limbs[i] > b->limbs[i]) return 1;
    if (a->limbs[i] < b->limbs[i]) return -1;
  }

  return 0; // Equal
}

int b64_b64_sub(struct Base2_64Int *a, const struct Base2_64Int *b) {
  /*
  * Subtracts two Base2_64Int numbers: a = a - b (in-place)
  * PRECONDITION: |a| >= |b|
  */
  if (a == NULL || b == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to b64_b64_sub\n");
    return -1;
  }

  uint64_t borrow = 0;
  for (size_t i = 0; i < a->len; i++) {
    uint64_t limb_a = a->limbs[i];
    uint64_t limb_b = (i < b->len) ? b->limbs[i] : 0;

    uint128_t sub = (uint128_t)limb_a - (uint128_t)limb_b - (uint128_t)borrow;

    a->limbs[i] = (uint64_t)sub;
    borrow = (sub >> 127) & 1; // If the subtraction underflowed, set borrow to 1
  }

  // Adjust length
  while (a->len > 0 && a->limbs[a->len - 1] == 0) {
    a->len--;
  }

  return 0;
}

int b64_b64_add(struct Base2_64Int *a, const struct Base2_64Int *b) {
  /*
  * Adds two Base2_64Int numbers: a = a + b (in-place)
  */
  if (a == NULL || b == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to b64_b64_add\n");
    return -1;
  }

  if (a->sign != b->sign) {
    int comp = b64_b64_cmp(a, b);
    if (comp == 0) {
      // Result is zero
      a->len = 0;
      a->sign = 0;
      memset(a->limbs, 0, a->capacity * sizeof(uint64_t));
    } else if (comp > 0) {
      // |a| > |b| => a = a - b
      if (b64_b64_sub(a, b) != 0) {
        return -1;
      }
    } else {
      // |a| < |b| => a = b - a, sign = b.sign
      struct Base2_64Int temp;
      if (b64_copy(&temp, b) != 0 && b64_b64_sub(&temp, a) != 0) {
        return -1;
      }
      b64_free(a);
      *a = temp;
      a->sign = 1;
    }
    return 0;
  }

  size_t max_len = (a->len > b->len) ? a->len : b->len;
  if (a->len < max_len + 1) {
    if (b64_expand(a, max_len + 1) != 0) {
      return -1;
    }
  }

  uint64_t carry = 0;
  for (size_t i = 0; i < max_len || carry; i++) {
    uint64_t limb_a = (i < a->len) ? a->limbs[i] : 0;
    uint64_t limb_b = (i < b->len) ? b->limbs[i] : 0;

    uint128_t sum = (uint128_t)limb_a + (uint128_t)limb_b + (uint128_t)carry;
    a->limbs[i] = (uint64_t)sum;
    carry = (uint64_t)(sum >> 64);
  }

  a->len = max_len + (carry ? 1 : 0);
  return 0;
}

int b64_mul(struct Base2_64Int *bn, uint64_t multiplier, uint64_t addend) {
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
