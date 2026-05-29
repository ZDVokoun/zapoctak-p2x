#ifndef ZAPOCTAK_RESIDUE_H
#define ZAPOCTAK_RESIDUE_H

#include "config.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Residue number system representation using Mersenne moduli
 */
struct ResidueInt {
  uint64_t *residues; /**< Array of residues for each modulus */
  size_t len;         /**< Number of moduli (length of residues array) */
};

/**
 * @brief Initialize a `ResidueInt` with enough moduli to represent minimumSz
 * bits
 *
 * @param res Output residue number to initialize
 * @param minimumSz Minimum number of bits to represent
 * @pre res != NULL
 * @note Asserts if moduli capacity is insufficient or memory allocation fails.
 */
void init_residue(struct ResidueInt *res, size_t minimumSz);

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
 * @pre dst != NULL && src != NULL
 * @note Asserts on memory allocation failure.
 */
void residue_copy(struct ResidueInt *dst, const struct ResidueInt *src);

/**
 * @brief Add two residue numbers: a = a + b (in-place)
 *
 * @param a Left operand and output (in-place)
 * @param b Right operand
 * @pre a != NULL && b != NULL && a->len == b->len
 * @note Time complexity: \f$\mathcal{O}(N)\f$ where \f$N\f$ is the number of moduli (a->len)
 */
void residue_add(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * @brief Subtract two residue numbers: a = a - b (in-place)
 *
 * @param a Left operand and output (in-place)
 * @param b Right operand
 * @pre a != NULL && b != NULL && a->len == b->len
 * @note Time complexity: \f$\mathcal{O}(N)\f$ where \f$N\f$ is the number of moduli (a->len)
 */
void residue_sub(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * @brief Multiply two residue numbers: a = a * b (in-place)
 *
 * @param a Left operand and output (in-place)
 * @param b Right operand
 * @pre a != NULL && b != NULL && a->len == b->len
 * @note Time complexity: \f$\mathcal{O}(N)\f$ where \f$N\f$ is the number of moduli (a->len)
 */
void residue_mul(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * @brief Compare two residue representations
 *
 * The comparison is done by converting both residues to mixed radix
 * representation and comparing the resulting values.
 *
 * @param a Left operand
 * @param b Right operand
 * @return -1 if a < b, 0 if a == b, and 1 if a > b.
 * @pre a->len == b->len
 * @note Time complexity: \f$\mathcal{O}(N)\f$ where \f$N\f$ is the number of moduli (a->len)
 */
int residue_cmp(const struct ResidueInt *a, const struct ResidueInt *b);

/**
 * @brief Print `ResidueInt` representation (debug)
 *
 * @param res Residue number to print
 */
void print_residue(const struct ResidueInt *res);

#endif // ZAPOCTAK_RESIDUE_H
