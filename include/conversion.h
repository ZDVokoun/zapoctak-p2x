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

#endif // ZAPOCTAK_CONVERSION_H
