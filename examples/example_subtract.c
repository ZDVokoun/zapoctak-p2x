#include <stdio.h>
#include "conversion.h"
#include "mp_number.h"
#include "residue.h"

/**
 * Example: Subtract two large decimal numbers using residue number system
 * Demonstrates the full pipeline:
 * 1. Parse decimal string to Base 2^64
 * 2. Convert to residue representation
 * 3. Perform subtraction in residue domain
 * 4. Output as decimal string
 */
int main() {
  struct ResidueInt res1, res2;

  // Example: 987654321987654321123456789 - 123456789123456789987654321
  const char *num1_str = "987654321987654321123456789";
  const char *num2_str = "123456789123456789987654321";

  printf("=== Multi-Precision Arithmetic using Residue Number System ===\n\n");

  printf("Converting '%s' to Residue representation...\n", num1_str);
  decimal_string_to_residue(num1_str, 0, &res1);
  print_residue(&res1);

  printf("\nConverting '%s' to Residue representation...\n", num2_str);
  decimal_string_to_residue(num2_str, 0, &res2);
  print_residue(&res2);

  printf("\n=== Performing Subtraction in Residue Domain ===\n");
  residue_sub(&res1, &res2);
  printf("Subtraction completed\n");
  print_residue(&res1);

  printf("\n=== Converting Result Back to Decimal ===\n");
  char result_str[4096];
  residue_to_decimal_string(&res1, result_str);
  printf("Result: %s\n", result_str);
  printf("Expected: 864197532864197531135802468\n");

  // Cleanup
  residue_free(&res1);
  residue_free(&res2);

  return 0;
}
