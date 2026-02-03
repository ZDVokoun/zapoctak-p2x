#include "config.h"
#include "residue.h"
#include "mp_number.h"
#include "conversion.h"
#include <stdlib.h>
#include <stdio.h>

int init_residue(struct ResidueInt *res, size_t minimumSz) {
  if (res == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to init_residue\n");
    return -1;
  }
  
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
    fprintf(stderr, "Error: Insufficient moduli capacity for required size %zu bits\n", minimumSz);
    return -1;
  }

  
  res->residues = calloc(moduliCount, sizeof(uint64_t));
  if (res->residues == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in init_residue\n");
    return -1;
  }
  res->len = moduliCount;
  
  return 0;
}

void residue_free(struct ResidueInt *res) {
  if (res == NULL) {
    return;
  }
  free(res->residues);
  res->residues = NULL;
  res->len = 0;
}

int residue_copy(struct ResidueInt *dst, const struct ResidueInt *src) {
  if (dst == NULL || src == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to residue_copy\n");
    return -1;
  }
  
  dst->len = src->len;
  dst->residues = malloc(dst->len * sizeof(uint64_t));
  if (dst->residues == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in residue_copy\n");
    dst->len = 0;
    return -1;
  }

  for (size_t i = 0; i < src->len; i++) {
    dst->residues[i] = src->residues[i];
  }
  
  return 0;
}

int residue_add(const struct ResidueInt *a, const struct ResidueInt *b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to residue_add\n");
    return -1;
  }
  
  if (a->len != b->len) {
    fprintf(stderr, "Error: Length mismatch in residue_add (a->len=%zu, b->len=%zu)\n", 
            a->len, b->len);
    return -1;
  }

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (1ULL << moduli64[i]) - 1;
    a->residues[i] = (a->residues[i] + b->residues[i]) % modulus;
  }
  
  return 0;
}

int residue_sub(const struct ResidueInt *a, const struct ResidueInt *b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to residue_sub\n");
    return -1;
  }
  
  if (a->len != b->len) {
    fprintf(stderr, "Error: Length mismatch in residue_sub (a->len=%zu, b->len=%zu)\n", 
            a->len, b->len);
    return -1;
  }

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (1ULL << moduli64[i]) - 1;
    a->residues[i] = (a->residues[i] + modulus - b->residues[i]) % modulus;
  }
  
  return 0;
}

int residue_mul(const struct ResidueInt *a, const struct ResidueInt *b) {
  if (a == NULL || b == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to residue_mul\n");
    return -1;
  }
  
  if (a->len != b->len) {
    fprintf(stderr, "Error: Length mismatch in residue_mul (a->len=%zu, b->len=%zu)\n", 
            a->len, b->len);
    return -1;
  }

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (1ULL << moduli64[i]) - 1;
    uint128_t product = (uint128_t)a->residues[i] * (uint128_t)b->residues[i];
    a->residues[i] = (uint64_t)(product % modulus);
  }
  
  return 0;
}

int residue_cmp(const struct ResidueInt *a, const struct ResidueInt *b, int *result) {
  /**
   * Compares two residue representations.
   * The comparison is done by converting both residues to mixed radix representation
   * and comparing the resulting values. The function sets *result to -1 if a < b,
   * 0 if a == b, and 1 if a > b.
   * Returns: 0 on success, -1 on error
   */
  if (a == NULL || b == NULL || result == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to residue_cmp\n");
    return -1;
  }
  
  if (a->len != b->len) {
    fprintf(stderr, "Error: Length mismatch in residue_cmp (a->len=%zu, b->len=%zu)\n", 
            a->len, b->len);
    return -1;
  }

  uint64_t v_a[a->len];
  uint64_t v_b[b->len];
  
  if (residue_to_mixed_radix(a, v_a) != 0) {
    return -1;
  }
  if (residue_to_mixed_radix(b, v_b) != 0) {
    return -1;
  }

  for (size_t i = a->len; i-- > 0;) {
    if (v_a[i] < v_b[i]) {
      *result = -1;
      return 0;
    } else if (v_a[i] > v_b[i]) {
      *result = 1;
      return 0;
    }
  }
  
  *result = 0;
  return 0;
}

void print_residue(const struct ResidueInt *res) {
  if (res == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to print_residue\n");
    return;
  }
  
  printf("\nResidue representation:\n");
  for (size_t i = 0; i < res->len; i++) {
    printf("Modulus %zu (2^%lu - 1): Residue %lu\n", i, moduli64[i],
           res->residues[i]);
  }
}
