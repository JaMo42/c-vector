#include <stdint.h>
#include "smallunit.h"
#include "vector.h"
SU_SOURCE;

#define ITERATIONS 20

uint32_t next_power_of_2(uint32_t v) {
  --v;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  ++v;
  return v;
}

void print_vector(int *data) {
  for (size_t i = 0; i < vector_size(data); ++i) {
    printf("%2zu: %d\n", i, data[i]);
  }
}

su_module(vector_tests, {
  int *ivec = NULL;

  su_test(vector_create, {
    int *a = NULL;
    vector_reserve(a, 10);
    int *b = vector_create(int, 10);
    su_assert(!memcmp(vector__get(a), vector__get(b), sizeof(struct vector__header)));
    vector_free(a);
    vector_free(b);
  })

  su_test(vector_push, {
    for (int i = 0; i < ITERATIONS; ++i) {
      vector_push(ivec, i);
    }
    su_assert_eq(vector_size(ivec), ITERATIONS);
    su_assert_eq(vector_capacity(ivec), next_power_of_2(ITERATIONS));
    for (int i = 0; i < ITERATIONS; ++i) {
      su_assert_eq(ivec[i], i);
    }
  })

  su_test(vector_pop, {
    su_assert_eq(vector_pop(ivec), ITERATIONS - 1);
    for (int i = 0; i < (ITERATIONS/2 - 2); ++i) {
      (void)vector_pop(ivec);
    }
    su_assert_eq(vector_pop(ivec), ITERATIONS - ITERATIONS/2);
    su_assert_eq(vector_size(ivec), ITERATIONS/2);
  })

  su_test(vector_insert, {
    vector_insert(ivec, 8, 30);
    vector_insert(ivec, 4, 20);
    vector_insert(ivec, 2, 10);
    su_assert_eq(vector_size(ivec), ITERATIONS/2 + 3);
    su_assert_eq(ivec[10], 30);
    su_assert_eq(ivec[5], 20);
    su_assert_eq(ivec[2], 10);
  })

  su_test(vector_remove, {
    vector_remove(ivec, 9);
    vector_remove(ivec, 4);
    vector_remove(ivec, 1);
    su_assert_eq(ivec[7], 30);
    su_assert_eq(ivec[3], 20);
    su_assert_eq(ivec[1], 10);
  })

  su_test(vector_shrink_to_fit, {
    vector_shrink_to_fit(ivec);
    su_assert_eq(vector_size(ivec), vector_capacity(ivec));
  })

  vector_free(ivec);
})

int main() {
  su_run_module(vector_tests);
}

