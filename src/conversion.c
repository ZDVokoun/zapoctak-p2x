#include "config.h"
#include "conversion.h"
#include "mp_number.h"
#include "residue.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define POW_TEN 19
#define TEN_POW_19 10000000000000000000ULL

int decimal_string_to_base2_64(const char *str, struct Base2_64Int *result) {
  /*
   * Converts a decimal string to Base 2^64 representation.
   * This method of multiple-precision conversion is shortly described in
   * "The Art of Computer Programming, Volume 2: Seminumerical Algorithms"
   * by Donald E. Knuth, Section 4.4, E. Multiple-precision conversion
   * Returns: 0 on success, -1 on error
   */
  if (str == NULL || result == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to decimal_string_to_base2_64\n");
    return -1;
  }
  
  size_t len = strlen(str);
  const char *p = str;

  // log2(10) is smaller than 10/3 because log2(10) = 3.322
  // We have to ceil the division result
  size_t initSz = (len * 10 + 2) / 3 / 64 + 1;
  if (b64_init(result, initSz) != 0) {
    return -1;
  }

  // Skip leading whitespace/zeros if necessary and handle sign
  while (isspace(*p)) {
    p++;
    len--;
  }
  if (*p == '-') {
    result->sign = 1;
    p++;
    len--;
  }
  while(*p == '0') {
    p++;
    len--;
  }
  if (len == 0)
    return 0;

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

    if (b64_mul(result, TEN_POW_19, chunk_val) != 0) {
      fprintf(stderr, "Error: Multiplication failed in decimal_string_to_base2_64\n");
      b64_free(result);
      return -1;
    }

    p += POW_TEN;
  }
  
  return 0;
}

int base2_64_to_residue(const struct Base2_64Int *bn, size_t minimumSz,
                         struct ResidueInt *res) {
  /*
   * Converts a Base 2^64 integer to its residue representation.
   * The conversion algorithm utilizes the special form of Mersenne moduli
   * and their properties described in "The Art of Computer Programming, Volume
   * 2: Seminumerical Algorithms" by Donald E. Knuth, Section 4.3.2., Modular
   * Arithmetic
   * Returns: 0 on success, -1 on error
   */

  if (bn == NULL || res == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to base2_64_to_residue\n");
    return -1;
  }
  
  if (bn->len * 64 + 1 > minimumSz)
    minimumSz = bn->len * 64 + 1;
  
  if (init_residue(res, minimumSz) != 0) {
    return -1;
  }

  struct Base2_64Int temp_bn;
  if (b64_copy(&temp_bn, bn) != 0) {
    residue_free(res);
    return -1;
  }
  b64_b64_add(&temp_bn, &res->range);

  for (size_t i = 0; i < res->len; i++) {
    uint64_t modulusPow = moduli64[i];
    uint64_t residue = 0;
    uint64_t modulus = (1ULL << modulusPow) - 1;

    size_t current_bit = 0;
    size_t limb_index = 0;

    for (size_t j = 0; j < temp_bn.len * 64; j += modulusPow) {

      if (current_bit >= 64) {
        current_bit -= 64;
        limb_index++;
      }
      if (limb_index >= temp_bn.len) {
        break;
      }
      // printf("Current bit: %zu Limb index: %zu\n", current_bit, limb_index);
      if (current_bit + modulusPow > 64) {
        uint64_t lower_part, upper_part = 0;
        lower_part = (temp_bn.limbs[limb_index] >> current_bit) & modulus;
        if (temp_bn.len > limb_index + 1) {
          upper_part =
              temp_bn.limbs[limb_index + 1] & (modulus >> (64 - current_bit));
        }
        residue += lower_part | (upper_part << (64 - current_bit));

      } else {
        residue += (temp_bn.limbs[limb_index] >> current_bit) & modulus;
      }
      residue %= modulus;

      current_bit += modulusPow;
    }

    res->residues[i] = residue;
  }

  b64_free(&temp_bn);
  
  return 0;
}

int residue_to_base2_64(const struct ResidueInt *res, struct Base2_64Int *bn) { 
  /*
   * Converts a residue representation back to Base 2^64 integer.
   * The conversion algorithm uses mixed radix system conversion.
   * Returns: 0 on success, -1 on error
   */

  if (res == NULL || bn == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to residue_to_base2_64\n");
    return -1;
  }

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
  if (b64_init(bn, 1) != 0) {
    return -1;
  }

  for (size_t i = res->len; i-- > 0;) {
    uint64_t modulus = precomputed_moduli[i];
    if (b64_mul(bn, modulus, 0) != 0 || b64_mul(bn, 1, v[i]) != 0) {
      fprintf(stderr, "Error: Multiplication failed in residue_to_base2_64\n");
      b64_free(bn);
      return -1;
    }
  }

  struct Base2_64Int bias;
  if (b64_copy(&bias, &res->range) != 0) {
    b64_free(bn);
    return -1;
  }
  b64_mul(&bias, 1, 1); // Add 1 before dividing by 2 (ceiling for odd numbers)
  base2_64_divmod(&bias, 2, NULL); // Divide by 2 to get bias for signed representation

  if (b64_b64_cmp(bn, &bias) > 0) {
    // Because we have a number greater than the bias, the result is negative
    b64_b64_sub(bn, &res->range);
    bn->sign = 1;
  }
  b64_free(&bias);
  return 0;
}

int base2_64_decimal_string(const struct Base2_64Int *bn, char *str) {
  if (bn == NULL || str == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to base2_64_decimal_string\n");
    return -1;
  }
  
  struct Base2_64Int temp;
  if (b64_init(&temp, bn->capacity) != 0) {
    return -1;
  }
  
  memcpy(temp.limbs, bn->limbs, bn->len * sizeof(uint64_t));
  temp.len = bn->len;

  size_t result_size = 20 * temp.len + 1;

  char buffer[result_size];
  char *p = buffer;
  // *--p = '\0';

  uint64_t current_rem = 0;
  while (temp.len > 0) {
    uint64_t rem;
    if (base2_64_divmod(&temp, TEN_POW_19, &rem) != 0) {
      fprintf(stderr, "Error: Division failed in base2_64_decimal_string\n");
      b64_free(&temp);
      return -1;
    }
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
  strcpy(str, buffer);
  b64_free(&temp);
  
  return 0;
}
