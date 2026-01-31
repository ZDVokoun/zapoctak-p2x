#include <stdio.h>
#include "conversion.h"
#include "mp_number.h"
#include "residue.h"

/**
 * Example: Add two large decimal numbers using residue number system
 * Demonstrates the full pipeline:
 * 1. Parse decimal string to Base 2^64
 * 2. Convert to residue representation
 * 3. Perform addition in residue domain
 * 4. Convert back to Base 2^64
 * 5. Output as decimal string
 */
int main() {
  struct Base2_64Int bn1, bn2, result;
  struct ResidueInt res1, res2;

  // Example: 123456789123456789987654321 + 987654321987654321123456789
  const char *num1_str = "123456789123456789987654321";
  const char *num2_str = "987654321987654321123456789";

  printf("=== Multi-Precision Arithmetic using Residue Number System ===\n\n");

  printf("Converting '%s' to Base 2^64...\n", num1_str);
  decimal_string_to_base2_64(num1_str, &bn1);
  print_base2_64(&bn1);

  printf("\nConverting '%s' to Base 2^64...\n", num2_str);
  decimal_string_to_base2_64(num2_str, &bn2);
  print_base2_64(&bn2);

  printf("\n=== Converting to Residue Representation ===\n");

  // Estimate required bits for result (both numbers have similar size)
  size_t required_bits = bn1.len * 64 + bn2.len * 64;

  printf("Converting first number to residue representation...\n");
  base2_64_to_residue(&bn1, required_bits, &res1);
  print_residue(&res1);

  printf("\nConverting second number to residue representation...\n");
  base2_64_to_residue(&bn2, required_bits, &res2);
  print_residue(&res2);

  printf("\n=== Performing Addition in Residue Domain ===\n");
  residue_add(&res1, &res2);
  printf("Addition completed\n");
  print_residue(&res1);

  printf("\n=== Converting Result Back to Base 2^64 ===\n");
  residue_to_base2_64(&res1, &result);
  print_base2_64(&result);

  printf("\n=== Final Result ===\n");
  char result_str[4096];
  base2_64_decimal_string(&result, result_str);
  printf("Result: %s\n", result_str);
  printf("Expected: 1111111111111111111111111110\n");

  // Cleanup
  b64_free(&bn1);
  b64_free(&bn2);
  b64_free(&result);
  residue_free(&res1);
  residue_free(&res2);

  return 0;
}
