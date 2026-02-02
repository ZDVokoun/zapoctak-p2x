#ifndef ZAPOCTAK_RESIDUE_H
#define ZAPOCTAK_RESIDUE_H

#include <stddef.h>
#include <stdint.h>
#include "config.h"

/**
 * Residue number system representation using Mersenne moduli
 */
struct ResidueInt {
  uint64_t *residues;
  size_t len;
};

/**
 * Initialize a ResidueInt with enough moduli to represent minimumSz bits
 * Returns: 0 on success, -1 on memory allocation failure or insufficient moduli capacity
 */
int init_residue(struct ResidueInt *res, size_t minimumSz);

/**
 * Free resources allocated for a ResidueInt
 */
void residue_free(struct ResidueInt *res);

/**
 * Copy a ResidueInt number
 * Returns: 0 on success, -1 on memory allocation failure
 */
int residue_copy(struct ResidueInt *dst, const struct ResidueInt *src);

/**
 * Add two residue numbers: a = a + b (in-place)
 * PRECONDITION: a->len == b->len
 * Returns: 0 on success, -1 if lengths don't match or pointers are NULL
 */
int residue_add(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * Subtract two residue numbers: a = a - b (in-place)
 * PRECONDITION: a->len == b->len
 * Returns: 0 on success, -1 if lengths don't match or pointers are NULL
 */
int residue_sub(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * Multiply two residue numbers: a = a * b (in-place)
 * PRECONDITION: a->len == b->len
 * Returns: 0 on success, -1 if lengths don't match or pointers are NULL
 */
int residue_mul(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * Print ResidueInt representation (debug)
 */
void print_residue(const struct ResidueInt *res);

#endif // ZAPOCTAK_RESIDUE_H
