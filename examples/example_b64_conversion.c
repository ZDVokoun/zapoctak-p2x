#include <stdio.h>
#include <string.h>
#include "conversion.h"
#include "mp_number.h"
#include "residue.h"

/**
 * Example: Demonstrate multi-precision number conversions
 * Shows how to convert between decimal strings and Base 2^64 representation
 */
int main() {
  struct Base2_64Int bn;

  // Examples with different sized numbers
  const char *examples[] = {
      "42",
      "1234567890",
      "18446744073709551615",  // 2^64 - 1
      "18446744073709551616",  // 2^64
      "12345678901234567890123456789",
  };

  int num_examples = sizeof(examples) / sizeof(examples[0]);

  printf("=== Multi-Precision Decimal to Base 2^64 Conversion ===\n\n");

  for (int i = 0; i < num_examples; i++) {
    printf("--- Example %d: %s ---\n", i + 1, examples[i]);

    decimal_string_to_base2_64(examples[i], &bn);
    print_base2_64(&bn);

    printf("Number of bits: %zu\n\n", bn.len * 64);

    b64_free(&bn);
  }

  return 0;
}
