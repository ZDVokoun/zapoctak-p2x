#ifndef ZAPOCTAK_CONVERSION_H
#define ZAPOCTAK_CONVERSION_H

#include "mp_number.h"
#include "residue.h"

/**
 * Convert a decimal string to Base 2^64 representation
 * Based on Knuth TAOCP Vol. 2, Section 4.4, Multiple-precision conversion
 * Returns: 0 on success, -1 on error
 */
int decimal_string_to_base2_64(const char *str, struct Base2_64Int *result);

/**
 * Convert Base 2^64 integer to residue representation
 * Utilizes properties of Mersenne moduli
 * Based on Knuth TAOCP Vol. 2, Section 4.3.2, Modular Arithmetic
 * Returns: 0 on success, -1 on error
 */
int base2_64_to_residue(const struct Base2_64Int *bn, size_t minimumSz,
                         struct ResidueInt *res);

/**
 * Convert decimal string to residue representation
 * First converts to Base 2^64 integer, then to residue
 * Returns: 0 on success, -1 on error
 */
int decimal_string_to_residue(const char *str, size_t minimumSz,
                           struct ResidueInt *res);

/**
 * Convert residue representation to mixed radix representation
 * Helper function for residue to Base 2^64 conversion and for comparators.
 * Returns: 0 on success, -1 on error
 */
int residue_to_mixed_radix(const struct ResidueInt *res, uint64_t *v);                        

/**
 * Convert residue representation back to Base 2^64 integer
 * Uses mixed radix system conversion
 * Returns: 0 on success, -1 on error
 */
int residue_to_base2_64(const struct ResidueInt *res, struct Base2_64Int *bn);

/**
 * Convert Base 2^64 integer to decimal string representation
 * Returns: 0 on success, -1 on error
 */
int base2_64_decimal_string(const struct Base2_64Int *bn, char *str);

/**
 * Convert residue representation back to decimal string
 * First converts to Base 2^64 integer, then to decimal string
 * Returns: 0 on success, -1 on error
 */
int residue_to_decimal_string(const struct ResidueInt *res, char *str);

#endif // ZAPOCTAK_CONVERSION_H
