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
  * @pre str != NULL && result != NULL
  * @note Asserts on memory allocation failure during initialization and expansion.
  * @note Time complexity: \f$\mathcal{O}(S \cdot M)\f$ where \f$S\f$ is the length of the decimal string and \f$N\f$ is the number of limbs in the result.
  */
void decimal_string_to_base2_64(const char *str, struct Base2_64Int *result);

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
 * @pre bn != NULL && res != NULL
 * @note Time complexity: \f$\mathcal{O}(N \cdot M)\f$ where \f$N\f$ is the number of limbs in the Base 2^64 integer and \f$M\f$ is the number of moduli in the residue representation.
 */
void base2_64_to_residue(const struct Base2_64Int *bn, size_t minimumSz,
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
 * @pre str != NULL && res != NULL
 * @see decimal_string_to_base2_64 and base2_64_to_residue
 * @note Time complexity: \f$\mathcal{O}(S \cdot M)\f$ where \f$S\f$ is the length of the decimal string and \f$M\f$ is the number of moduli in the residue representation.
 */
void decimal_string_to_residue(const char *str, size_t minimumSz,
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
 * @pre res != NULL && v != NULL
 * @note Time complexity: \f$\mathcal{O}(M^2)\f$, where \f$M\f$ is the number of moduli in the residue representation, due to the nested loops for coefficient adjustment.
 */
void residue_to_mixed_radix(const struct ResidueInt *res, uint64_t *v);                        

/**
 * @brief Converts a residue representation back to Base 2^64 integer
 *
 * The conversion algorithm uses mixed radix system conversion to transform
 * residue coefficients back to standard positional notation.
 *
 * @param res Input residue representation
 * @param bn Output Base 2^64 integer
 * @pre res != NULL && bn != NULL
 * @see residue_to_mixed_radix
 * @note Time complexity: \f$\mathcal{O}(M^2 + N \cdot M)\f$ where \f$M\f$ is the number of moduli in the residue representation and \f$N\f$ is the number of limbs in the Base 2^64 integer.
 */
void residue_to_base2_64(const struct ResidueInt *res, struct Base2_64Int *bn);

/**
 * @brief Converts a Base 2^64 integer to decimal string representation
 *
 * Performs division-based conversion of Base 2^64 representation to a
 * decimal string using repeated division by 10^19.
 *
 * @param bn Input Base 2^64 integer
 * @param str Output buffer for decimal string (must be large enough)
 * @pre bn != NULL && str != NULL
 * @note Time complexity: \f$\mathcal{O}(bn->len)\f$
 */
void base2_64_decimal_string(const struct Base2_64Int *bn, char *str);

/**
 * @brief Converts a residue representation back to decimal string
 *
 * This function first converts the residue representation to Base 2^64
 * integer representation and then to decimal string.
 *
 * @param res Input residue representation
 * @param str Output buffer for decimal string (must be large enough)
 * @pre res != NULL && str != NULL
 * @see residue_to_base2_64 and base2_64_decimal_string
 * @note Time complexity: \f$\mathcal{O}(M^2 + S)\f$ where \f$M\f$ is the number of moduli in the residue representation and \f$S\f$ is the length of the output decimal string.
 */
void residue_to_decimal_string(const struct ResidueInt *res, char *str);

#endif // ZAPOCTAK_CONVERSION_H
