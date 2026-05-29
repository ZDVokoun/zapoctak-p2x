#include "residue.h"
#include "config.h"
#include "conversion.h"
#include "mp_number.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void init_residue(struct ResidueInt *res, size_t minimumSz) {
  assert(res != NULL);

  size_t moduliSum = 0;
  size_t moduliCount = 0;
  for (size_t i = 0; i < moduli64len; i++) {
    moduliSum += moduli64[i];
    if (moduliSum >= minimumSz) {
      moduliCount = i + 1;
      break;
    }
  }
  assert(moduliSum >= minimumSz);

  res->residues = calloc(moduliCount, sizeof(uint64_t));
  assert(res->residues != NULL || moduliCount == 0);
  res->len = moduliCount;
}

void residue_free(struct ResidueInt *res) {
  if (res == NULL) {
    return;
  }
  free(res->residues);
  res->residues = NULL;
  res->len = 0;
}

void residue_copy(struct ResidueInt *dst, const struct ResidueInt *src) {
  assert(dst != NULL);
  assert(src != NULL);

  dst->len = src->len;
  dst->residues = malloc(dst->len * sizeof(uint64_t));
  assert(dst->residues != NULL || dst->len == 0);

  for (size_t i = 0; i < src->len; i++) {
    dst->residues[i] = src->residues[i];
  }
}

void residue_add(const struct ResidueInt *a, const struct ResidueInt *b) {
  assert(a != NULL);
  assert(b != NULL);
  assert(a->len == b->len);

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (moduli64[i] == 64) ? ~0ULL : (1ULL << moduli64[i]) - 1;
    uint64_t sum = a->residues[i] + b->residues[i];
    uint64_t carry = (moduli64[i] == 64) ? (sum < a->residues[i]) : (sum >> moduli64[i]);
    a->residues[i] = (sum & modulus) + carry;
    if (a->residues[i] == modulus)
      a->residues[i] = 0;
  }
}

void residue_sub(const struct ResidueInt *a, const struct ResidueInt *b) {
  assert(a != NULL);
  assert(b != NULL);
  assert(a->len == b->len);

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (moduli64[i] == 64) ? ~0ULL : (1ULL << moduli64[i]) - 1;
    uint64_t diff = a->residues[i] - b->residues[i];
    if (a->residues[i] < b->residues[i]) {
      diff--;
    }
    a->residues[i] = diff & modulus;
    if (a->residues[i] == modulus)
      a->residues[i] = 0;
  }
}

void residue_mul(const struct ResidueInt *a, const struct ResidueInt *b) {
  assert(a != NULL);
  assert(b != NULL);
  assert(a->len == b->len);

  for (size_t i = 0; i < a->len; i++) {
    uint64_t modulus = (moduli64[i] == 64) ? ~0ULL : (1ULL << moduli64[i]) - 1;
    uint128_t product = (uint128_t)a->residues[i] * (uint128_t)b->residues[i];
    
    uint64_t low = (uint64_t)product & modulus;
    uint64_t high = (uint64_t)(product >> moduli64[i]);
    
    uint64_t res = low + high;
    uint64_t carry = (moduli64[i] == 64) ? (res < low) : (res >> moduli64[i]);
    
    a->residues[i] = (res & modulus) + carry;
    if (a->residues[i] == modulus)
      a->residues[i] = 0;
  }
}

int residue_cmp(const struct ResidueInt *a, const struct ResidueInt *b) {
  assert(a != NULL);
  assert(b != NULL);
  assert(a->len == b->len);

  // Convert residue representation to mixed radix system
  // in which the numbers can be compared lexicographically

  uint64_t v_a[a->len];
  uint64_t v_b[b->len];

  residue_to_mixed_radix(a, v_a);
  residue_to_mixed_radix(b, v_b);

  for (size_t i = a->len; i-- > 0;) {
    if (v_a[i] < v_b[i]) {
      return -1;
    } else if (v_a[i] > v_b[i]) {
      return 1;
    }
  }

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
