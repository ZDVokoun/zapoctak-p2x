#ifndef ZAPOCTAK_RESIDUE_H
#define ZAPOCTAK_RESIDUE_H

#include <stddef.h>
#include <stdint.h>
#include "config.h"

/**
 * @brief Residue number system representation using Mersenne moduli
 */
struct ResidueInt {
  uint64_t *residues;
  size_t len;
};

/**
 * @brief Initialize a `ResidueInt` with enough moduli to represent minimumSz bits
 *
 * @param res Output residue number to initialize
 * @param minimumSz Minimum number of bits to represent
 * @return 0 on success
 * @retval -1 if memory allocation fails or moduli capacity is insufficient
 */
int init_residue(struct ResidueInt *res, size_t minimumSz);

/**
 * @brief Free resources allocated for a `ResidueInt`
 *
 * @param res Residue number to free
 */
void residue_free(struct ResidueInt *res);

/**
 * @brief Copy a `ResidueInt` number
 *
 * @param dst Destination residue number
 * @param src Source residue number
 * @return 0 on success
 * @retval -1 if memory allocation fails
 */
int residue_copy(struct ResidueInt *dst, const struct ResidueInt *src);

/**
 * @brief Add two residue numbers: a = a + b (in-place)
 *
 * @param a Left operand and output (in-place)
 * @param b Right operand
 * @return 0 on success
 * @retval -1 if lengths don't match or pointers are NULL
 * @pre a->len == b->len
 */
int residue_add(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * @brief Subtract two residue numbers: a = a - b (in-place)
 *
 * @param a Left operand and output (in-place)
 * @param b Right operand
 * @return 0 on success
 * @retval -1 if lengths don't match or pointers are NULL
 * @pre a->len == b->len
 */
int residue_sub(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * @brief Multiply two residue numbers: a = a * b (in-place)
 *
 * @param a Left operand and output (in-place)
 * @param b Right operand
 * @return 0 on success
 * @retval -1 if lengths don't match or pointers are NULL
 * @pre a->len == b->len
 */
int residue_mul(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * @brief Compare two residue representations
 *
 * The comparison is done by converting both residues to mixed radix
 * representation and comparing the resulting values. The function sets
 * *result to -1 if a < b, 0 if a == b, and 1 if a > b.
 *
 * @param a Left operand
 * @param b Right operand
 * @param result Output comparison result (-1, 0, 1)
 * @return 0 on success
 * @retval -1 on error
 */
int residue_cmp(const struct ResidueInt *a, const struct ResidueInt *b, int *result);

/**
 * @brief Print `ResidueInt` representation (debug)
 *
 * @param res Residue number to print
 */
void print_residue(const struct ResidueInt *res);

#endif // ZAPOCTAK_RESIDUE_H
