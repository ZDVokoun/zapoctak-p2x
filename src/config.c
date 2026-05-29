#include "config.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

// Define global configuration
size_t moduli64len = 0;
uint64_t moduli64[18] = {0};

static uint64_t gcd(uint64_t a, uint64_t b) {
  while (b) {
    a %= b;
    uint64_t tmp = a;
    a = b;
    b = tmp;
  }
  return a;
}

void init_moduli_greedy(uint64_t n) {
  assert(n >= 2);
  assert(n <= 64);
  moduli64[0] = n;
  moduli64len = 1;

  for (uint64_t k = n - 1; k >= 2 && moduli64len < 18; k--) {
    bool coprime = true;
    for (size_t i = 0; i < moduli64len; i++) {
      if (gcd(k, moduli64[i]) != 1) {
        coprime = false;
        break;
      }
    }
    if (coprime) {
      moduli64[moduli64len++] = k;
    }
  }
}

void init_moduli_prime_powers(uint64_t n) {
  assert(n >= 2);
  assert(n <= 64);

  bool *is_prime = malloc((n + 1) * sizeof(bool));
  assert(is_prime != NULL);

  for (uint64_t i = 0; i <= n; i++)
    is_prime[i] = true;
  is_prime[0] = is_prime[1] = false;

  for (uint64_t p = 2; p * p <= n; p++) {
    if (is_prime[p]) {
      for (uint64_t i = p * p; i <= n; i += p)
        is_prime[i] = false;
    }
  }

  moduli64len = 0;
  for (uint64_t p = 2; p <= n && moduli64len < 18; p++) {
    if (is_prime[p]) {
      uint64_t m = p;
      while (n / p >= m) {
        m *= p;
      }
      moduli64[moduli64len++] = m;
    }
  }

  // Sort descending
  for (size_t i = 0; i < moduli64len; i++) {
    for (size_t j = i + 1; j < moduli64len; j++) {
      if (moduli64[i] < moduli64[j]) {
        uint64_t tmp = moduli64[i];
        moduli64[i] = moduli64[j];
        moduli64[j] = tmp;
      }
    }
  }

  free(is_prime);
}
