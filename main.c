#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if BITINT_MAXWIDTH < 128
#error                                                                         \
    "This C23 compiler does not support 128-bit integers needed for fast Base 2^64 arithmetic."
#endif

#define POW_TEN 19
#define TEN_POW_19 10000000000000000000ULL

typedef unsigned _BitInt(128) uint128_t;

// Defining coprime Mersenne moduli
size_t moduli64len = 15;
uint64_t moduli64[15] = {63, 61, 59, 55, 53, 47, 43, 41,
                         37, 31, 29, 23, 19, 17, 13};

// size_t moduli32len = 10;
// uint32_t moduli32[10] = {31, 29, 27, 25, 23, 19, 17, 13, 11, 7};

struct Base2_64Int {
  uint64_t *limbs; // Limbs are stored in little-endian order
  size_t len;
  size_t capacity;
};

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
    printf("Limb %zu: %llu\n", i, bn->limbs[i]);
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

void decimal_string_to_base2_64(const char *str, struct Base2_64Int *result) {
  /*
   * Converts a decimal string to Base 2^64 representation.
   * This method of multiple-precision conversion is shortly described in
   * "The Art of Computer Programming, Volume 2: Seminumerical Algorithms"
   * by Donald E. Knuth, Section 4.4, E. Multiple-precision conversion
   */
  size_t len = strlen(str);
  const char *p = str;

  // log2(10) is smaller than 10/3 because log2(10) = 3.322
  // We have to ceil the division result
  size_t initSz = (len * 10 + 2) / 3 / 64 + 1;
  b64_init(result, initSz);

  // Skip leading whitespace/zeros if necessary
  while (*p == '0' || isspace(*p)) {
    p++;
    len--;
  }
  if (len == 0)
    return;

  // First chunk size is a remainder of chunk size
  int first_chunk_len = len % POW_TEN;
  if (first_chunk_len == 0)
    first_chunk_len = POW_TEN;

  char buffer[POW_TEN + 1];

  // Copy the chunk and convert it to decimal integer
  memcpy(buffer, p, first_chunk_len);
  buffer[first_chunk_len] = '\0';
  uint64_t val = strtoull(buffer, NULL, 10);

  // Add result of the first chunk
  if (val > 0) {
    result->limbs[0] = val;
    result->len = 1;
  }

  p += first_chunk_len;

  // Process remaining full chunks
  while (*p) {
    memcpy(buffer, p, POW_TEN);
    buffer[POW_TEN] = '\0';
    uint64_t chunk_val = strtoull(buffer, NULL, 10);

    b64_mul(result, TEN_POW_19, chunk_val);

    p += POW_TEN;
  }
}

struct ResidueInt {
  uint64_t *residues;
  size_t len;
};

void init_residue(struct ResidueInt *res, size_t minimumSz) {

  size_t moduliSum = 0;
  size_t moduliCount = 0;
  for (size_t i = 0; i < moduli64len; i++) {
    moduliSum += moduli64[i];
    if (moduliSum >= minimumSz) {
      moduliCount = i + 1;
      break;
    }
  }
  if (moduliSum < minimumSz) {
    // TODO Throw error
    return;
  }

  res->len = moduliCount;
  res->residues = malloc(res->len * sizeof(uint64_t));
}

void residue_free(struct ResidueInt *res) {
  free(res->residues);
  res->len = 0;
}

void base2_64_to_residue(const struct Base2_64Int *bn, size_t minimumSz,
                         struct ResidueInt *res) {
  /*
   * Converts a Base 2^64 integer to its residue representation.
   * The conversion algorithm utilizes the special form of Mersenne moduli
   * and their properties described in "The Art of Computer Programming, Volume
   * 2: Seminumerical Algorithms" by Donald E. Knuth, Section 4.3.2., Modular
   * Arithmetic
   */
  if (bn->len * 64 > minimumSz)
    minimumSz = bn->len * 64;
  init_residue(res, minimumSz);

  for (size_t i = 0; i < res->len; i++) {
    uint64_t modulusPow = moduli64[i];
    uint64_t residue = 0;
    uint64_t modulus = (1ULL << modulusPow) - 1;

    size_t current_bit = 0;
    size_t limb_index = 0;

    for (size_t j = 0; j < bn->len * 64; j += modulusPow) {

      if (current_bit >= 64) {
        current_bit -= 64;
        limb_index++;
      }
      if (limb_index >= bn->len) {
        break;
      }
      // printf("Current bit: %zu Limb index: %zu\n", current_bit, limb_index);
      if (current_bit + modulusPow > 64) {
        uint64_t lower_part, upper_part = 0;
        lower_part = (bn->limbs[limb_index] >> current_bit) & modulus;
        if (bn->len > limb_index + 1) {
          upper_part =
              bn->limbs[limb_index + 1] & (modulus >> (64 - current_bit));
        }
        residue += lower_part | (upper_part << (64 - current_bit));

      } else {
        residue += (bn->limbs[limb_index] >> current_bit) & modulus;
      }
      residue %= modulus;

      current_bit += modulusPow;
    }

    res->residues[i] = residue;
  }
}

void residue_add(const struct ResidueInt *a, const struct ResidueInt *b) {
  if (a->len != b->len) {
    // TODO Throw error
    return;
  }

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (1ULL << moduli64[i]) - 1;
    a->residues[i] = (a->residues[i] + b->residues[i]) % modulus;
  }
}
void residue_sub(const struct ResidueInt *a, const struct ResidueInt *b) {
  if (a->len != b->len) {
    // TODO Throw error
    return;
  }

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (1ULL << moduli64[i]) - 1;
    a->residues[i] = (a->residues[i] + modulus - b->residues[i]) % modulus;
  }
}

void residue_mul(const struct ResidueInt *a, const struct ResidueInt *b) {
  if (a->len != b->len) {
    // TODO Throw error
    return;
  }

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (1ULL << moduli64[i]) - 1;
    uint128_t product = (uint128_t)a->residues[i] * (uint128_t)b->residues[i];
    a->residues[i] = (uint64_t)(product % modulus);
  }
}

void print_residue(const struct ResidueInt *res) {
  printf("\nResidue representation:\n");
  for (size_t i = 0; i < res->len; i++) {
    printf("Modulus %zu (2^%llu - 1): Residue %llu\n", i, moduli64[i],
           res->residues[i]);
  }
}

void residue_to_base2_64(const struct ResidueInt *res, struct Base2_64Int *bn) {
  // TODO Implement the conversion from residue representation to Base 2^64

  // Representation of the integer in mixed radix system with bases being the
  // moduli It's going to be of a form u = v_0 + v_1 * m_0 + v_2 * m_0 * m_1 +
  // ... + v_n * m_0 * m_1 * ... * m_(n-1)
  uint64_t v[res->len];
  for (size_t i = 0; i < res->len; i++) {
    v[i] = res->residues[i];
  }

  uint64_t precomputed_moduli[res->len];
  for (size_t i = 0; i < res->len; i++) {
    precomputed_moduli[i] = (1ULL << moduli64[i]) - 1;
  }

  for (size_t j = 0; j < res->len; j++) {
    for (size_t i = 0; i < j; i++) {
      uint64_t modulus = precomputed_moduli[j];
      v[j] = (modulus + v[j] % modulus - v[i] % modulus) % modulus;

      // Solve c * m_i ≡ 1 (mod m_j) for c
      // Here I'm utilizing a result of the exercise 6 in Section 4.3.2 of
      // "The Art of Computer Programming, Volume 2: Seminumerical Algorithms"
      uint64_t c = 0;
      uint64_t d = moduli64[i] % moduli64[j];
      for (size_t k = 0; (k * moduli64[i]) % moduli64[j] != 1; k++) {
        c |= 1ULL << ((k * d) % moduli64[j]);
      }

      v[j] = ((uint128_t)v[j] * (uint128_t)c) % modulus;
    }
  }

  // Convertion to Base 2^64 representation
  b64_init(bn, 1);

  for (size_t i = res->len; i-- > 0;) {
    uint64_t modulus = precomputed_moduli[i];
    b64_mul(bn, modulus, 0);

    b64_mul(bn, 1, v[i]);
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

void base2_64_decimal_string(const struct Base2_64Int *bn, const char *str) {
  struct Base2_64Int temp;
  b64_init(&temp, bn->capacity);
  memcpy(temp.limbs, bn->limbs, bn->len * sizeof(uint64_t));
  temp.len = bn->len;

  size_t result_size = 20 * temp.len + 1;

  char buffer[result_size];
  char *p = buffer;
  // *--p = '\0';

  uint64_t current_rem = 0;
  while (temp.len > 0) {
    uint64_t rem;
    base2_64_divmod(&temp, TEN_POW_19, &rem);
    current_rem += rem;

    // Convert remainder to decimal string
    for (int i = 0; i < POW_TEN; i++) {
      if (temp.len == 0 && current_rem == 0) {
        break;
      }
      char digit = (char)(current_rem % 10);
      current_rem /= 10;
      *p++ = '0' + digit;
    }
  }
  *p = '\0';

  // reverse in-place
  size_t len = (size_t)(p - buffer);
  for (size_t i = 0; i < len / 2; i++) {
    char tmp = buffer[i];
    buffer[i] = buffer[len - 1 - i];
    buffer[len - 1 - i] = tmp;
  }
  strcpy((char *)str, buffer);
}

int main() {
  struct Base2_64Int bn;
  struct ResidueInt res1;
  struct ResidueInt res2;

  const char *decimal_str = "1234567890123456789012345678901234567890";

  decimal_string_to_base2_64(decimal_str, &bn);

  base2_64_to_residue(&bn, bn.len * 64 * 2, &res1);
  base2_64_to_residue(&bn, bn.len * 64 * 2, &res2);

  // base2_64_print_decimal(&bn);
  print_residue(&res1);
  print_residue(&res2);

  residue_mul(&res1, &res2);

  print_residue(&res1);

  struct Base2_64Int bn2;
  residue_to_base2_64(&res1, &bn2);
  print_base2_64(&bn2);
  const char result_str[4096];
  base2_64_decimal_string(&bn2, result_str);
  puts(result_str);

  b64_free(&bn);
  b64_free(&bn2);
  residue_free(&res1);
  residue_free(&res2);

  return 0;
}
