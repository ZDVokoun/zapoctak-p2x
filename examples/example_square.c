#include "conversion.h"
#include "mp_number.h"
#include "residue.h"
#include <stdio.h>

/**
 * Example: Square a large decimal number using residue number system
 * Demonstrates the full pipeline:
 * 1. Parse decimal string to Base 2^64
 * 2. Convert to residue representation
 * 3. Perform power operation in residue domain
 * 4. Convert back to Base 2^64
 * 5. Output as decimal string
 */
int main() {
  struct Base2_64Int bn;
  struct ResidueInt res1, res2;

  // Example: 123456789123456789987654321^2
  const char *num1_str = "123456789123456789987654321";

  printf("=== Multi-Precision Arithmetic using Residue Number System ===\n\n");

  printf("Converting '%s' to Base 2^64...\n", num1_str);
  decimal_string_to_base2_64(num1_str, &bn);
  print_base2_64(&bn);

  printf("\n=== Converting to Residue Representation ===\n");

  // Estimate required bits for result (both numbers have similar size)
  size_t required_bits = 2 * bn.len * 64;

  printf("Converting first number to residue representation...\n");
  base2_64_to_residue(&bn, required_bits, &res1);
  print_residue(&res1);

  printf("Making the Base 2^64 number zero...\n");
  b64_empty(&bn);
  print_base2_64(&bn);

  printf("Copy the number for power operation...\n");
  residue_copy(&res2, &res1);
  print_residue(&res2);

  printf("\n=== Performing Power Operation in Residue Domain ===\n");
  residue_mul(&res1, &res2);
  printf("Power operation completed\n");
  print_residue(&res1);

  printf("\n=== Converting Result Back to Base 2^64 ===\n");
  residue_to_base2_64(&res1, &bn);
  print_base2_64(&bn);

  printf("\n=== Final Result ===\n");
  char result_str[4096];
  base2_64_decimal_string(&bn, result_str);
  printf("Result: %s\n", result_str);
  printf("Expected: 15241578780673678759487883219326322200731595789971041\n");

  // Cleanup
  b64_free(&bn);
  residue_free(&res1);
  residue_free(&res2);

  return 0;
}
