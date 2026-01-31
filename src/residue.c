#include "config.h"
#include "residue.h"
#include <stdlib.h>
#include <stdio.h>

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

void residue_copy(struct ResidueInt *dst, const struct ResidueInt *src) {
  dst->len = src->len;
  dst->residues = malloc(dst->len * sizeof(uint64_t));

  for (size_t i = 0; i < src->len; i++) {
    dst->residues[i] = src->residues[i];
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
    printf("Modulus %zu (2^%lu - 1): Residue %lu\n", i, moduli64[i],
           res->residues[i]);
  }
}
