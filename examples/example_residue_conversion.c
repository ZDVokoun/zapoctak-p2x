#include <stdio.h>
#include <string.h>
#include "conversion.h"
#include "mp_number.h"
#include "residue.h"

/**
 * Example: Demonstrate residue number conversions
 * Shows how to convert between decimal strings and residue representation
 */
int main() {
  struct Base2_64Int bn;
  struct ResidueInt res;

  const char *example = "1234567890123456789012345678901234567890";

  printf("Example number: %s\n\n", example);

  printf("=== Multi-Precision Decimal to Base 2^64 Conversion ===\n\n");
  decimal_string_to_base2_64(example, &bn);
  print_base2_64(&bn);

  printf("=== Conversion to Residue Representation ===\n\n");
  base2_64_to_residue(&bn, 0, &res);
  printf("Residue representation:\n");
  print_residue(&res);

  printf("Expected residues based on moduli:\n");
  for (size_t i = 0; i < res.len; i++) {
    uint64_t modulus = (1ULL << moduli64[i]) - 1;
    struct Base2_64Int temp_bn;
    b64_copy(&temp_bn, &bn);
    uint64_t rem;
    base2_64_divmod(&temp_bn, modulus, &rem);
    printf("Residue %zu: %lu\n", i, rem);
  }

  printf("\n=== Conversion Back to Base 2^64 ===\n\n");
  struct Base2_64Int bn_converted;
  residue_to_base2_64(&res, &bn_converted);
  print_base2_64(&bn_converted);
  printf("Expected Base 2^64 representation:\n");
  print_base2_64(&bn);

  printf("\n=== Final Decimal Representation ===\n\n");
  char result_str[4096];
  base2_64_decimal_string(&bn_converted, result_str);
  printf("Converted back to decimal: %s\n", result_str);
  printf("Expected: %s\n", example);

  return 0;
}
