#ifndef ZAPOCTAK_CONVERSION_H
#define ZAPOCTAK_CONVERSION_H

#include "mp_number.h"
#include "residue.h"

/**
  * @brief Converts a decimal string to Base 2^64 representation.
  * 
  * This method of multiple-precision conversion is shortly described in
  * "The Art of Computer Programming, Volume 2: Seminumerical Algorithms"
  * by Donald E. Knuth, Section 4.4, E. Multiple-precision conversion.
  * 
  * @param str Input decimal string
  * @param result Output Base 2^64 integer
  * @return 0 on success, -1 on error
  */
int decimal_string_to_base2_64(const char *str, struct Base2_64Int *result);

/**
 * @brief Converts a Base 2^64 integer to its residue representation
 *
 * The conversion algorithm utilizes the special form of Mersenne moduli
 * and their properties described in "The Art of Computer Programming, Volume
 * 2: Seminumerical Algorithms" by Donald E. Knuth, Section 4.3.2., Modular
 * Arithmetic.
 *
 * @param bn Input Base 2^64 integer
 * @param minimumSz Minimum size for the residue representation
 * @param res Output residue representation
 * @return 0 on success
 * @retval -1 if bn or res is NULL, or if initialization fails
 */
int base2_64_to_residue(const struct Base2_64Int *bn, size_t minimumSz,
                         struct ResidueInt *res);

/**
 * @brief Converts a decimal string to its residue representation
 *
 * This function first converts the decimal string to Base 2^64 integer
 * representation and then to residue representation.
 *
 * @param str Input decimal string
 * @param minimumSz Minimum size for the residue representation
 * @param res Output residue representation
 * @return 0 on success
 * @retval -1 if str or res is NULL, or if conversion fails
 * @see decimal_string_to_base2_64 and base2_64_to_residue
 */
int decimal_string_to_residue(const char *str, size_t minimumSz,
                           struct ResidueInt *res);

/**
 * @brief Converts a residue representation to its mixed radix representation
 *
 * Helper function for the conversion from residue to Base 2^64
 * representation. The mixed radix system has bases being the moduli of the
 * residue representation, resulting in the form:
 * u = v_0 + v_1 * m_0 + v_2 * m_0 * m_1 + ... + v_n * m_0 * m_1 * ... * m_(n-1)
 *
 * @param res Input residue representation
 * @param v Output array for mixed radix coefficients
 * @return 0 on success
 * @retval -1 if res or v is NULL
 */
int residue_to_mixed_radix(const struct ResidueInt *res, uint64_t *v);                        

/**
 * @brief Converts a residue representation back to Base 2^64 integer
 *
 * The conversion algorithm uses mixed radix system conversion to transform
 * residue coefficients back to standard positional notation.
 *
 * @param res Input residue representation
 * @param bn Output Base 2^64 integer
 * @return 0 on success
 * @retval -1 if res or bn is NULL, or if conversion fails
 * @see residue_to_mixed_radix
 */
int residue_to_base2_64(const struct ResidueInt *res, struct Base2_64Int *bn);

/**
 * @brief Converts a Base 2^64 integer to decimal string representation
 *
 * Performs division-based conversion of Base 2^64 representation to a
 * decimal string using repeated division by 10^19.
 *
 * @param bn Input Base 2^64 integer
 * @param str Output buffer for decimal string (must be large enough)
 * @return 0 on success
 * @retval -1 if bn or str is NULL, or if division fails
 */
int base2_64_decimal_string(const struct Base2_64Int *bn, char *str);

/**
 * @brief Converts a residue representation back to decimal string
 *
 * This function first converts the residue representation to Base 2^64
 * integer representation and then to decimal string.
 *
 * @param res Input residue representation
 * @param str Output buffer for decimal string (must be large enough)
 * @return 0 on success
 * @retval -1 if res or str is NULL, or if conversion fails
 * @see residue_to_base2_64 and base2_64_decimal_string
 */
int residue_to_decimal_string(const struct ResidueInt *res, char *str);

#endif // ZAPOCTAK_CONVERSION_H
