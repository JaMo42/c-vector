#include <stdint.h>
#include <stdarg.h>
#include "smallunit.h"
#define VECTOR_IMPLEMENTATION
#include "vector.h"
#define VECTOR_IMPLEMENTATION
#include "static_vector.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wnull-pointer-arithmetic"
#endif

#define ITERATIONS 20

#define signof(n) (((n) > 0) - ((n) < 0))

struct MyStruct
{
  int i;
  float f;
  const char *s;
};

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

// Checks if the given integer vector has SIZE elements which are the same as
// the given variadic arguments.
int check(VECTOR(int) v, const size_t size, ...) {
  va_list ap;
  int my, expected;
  size_t i;
  if (vector_size (v) != size) {
    fprintf (stderr,
             "check: size mismatch: vector_size (v) != %zu\n"
             "                 with vector_size (v) = %zu\n",
             size, vector_size (v));
    return 0;
  }
  va_start (ap, size);
  for (i = 0; i < size; ++i) {
    expected = va_arg (ap, int);
    my = v[i];
    if (my != expected) {
      fprintf (stderr,
               "check: value mismatch: v[%zu] != %d\n"
               "                  with v[%zu] = %d\n",
               i, expected, i, my);
      va_end (ap);
      return 0;
    }
  }
  va_end (ap);
  return 1;
}

static int G_int_buffer[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

su_module (vector_tests, {
  int *ivec = NULL;

  su_test ("VECTOR(T)", {
    VECTOR(int) a = NULL;
    int *b = NULL;
    su_assert (_Generic(a, int *: 1, default: 0));
    su_assert_eq (a, b);
  })

  su_test("vector_create", {
    int *a = NULL;
    vector_reserve(a, 10);
    int *b = vector_create(int, 10);
    su_assert(!memcmp(vector__get(a), vector__get(b), sizeof(struct vector__header)));
    vector_free(a);
    vector_free(b);
  })

  su_test ("vector_create_from", {
    VECTOR(int) v = vector_create_from (G_int_buffer, 10);
    su_assert (check (v, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    vector_free (v);
  })

  su_test ("vector_idx", {
    VECTOR(int) v = vector_create_from (G_int_buffer, 10);
    su_assert_eq (vector_idx (v, 0), 0);
    su_assert_eq (vector_idx (v, -1), 9);
    su_assert_eq (vector_idx (NULL, 10), 10);
    su_assert_eq (vector_idx (NULL, -10), (size_t)-10);
    vector_free (v);
  })

  su_test ("vector_at", {
    VECTOR(int) v = vector_create_from (G_int_buffer, 10);
    su_assert_eq (vector_at (v, 0), v);
    su_assert_eq (vector_at (v, 100), NULL);
    su_assert_eq (*vector_at (v, -1), 9);
    su_assert_eq (vector_at (NULL, 0), NULL);
    su_assert_eq (vector_at (NULL, 1), NULL);
    su_assert_eq (vector_at (NULL, -1), NULL);
    vector_free (v);
  })

  su_test ("vector_reserve", {
    int *v = vector_create (int, 5);
    vector_reserve (v, 20);
    su_assert (vector_capacity (v) >= 20);
    vector_free (v);
  })

  su_test ("vector_compare", {
    {
      VECTOR(char) a = vector_init ((char)'a');
      VECTOR(char) ab = vector_init ((char)'a', 'b');
      VECTOR(char) b = vector_init ((char)'b');
      su_assert (vector_compare (a, a) == 0);
      su_assert (vector_compare (a, b) < 0);
      su_assert (vector_compare (b, a) > 0);
      su_assert (vector_compare (a, ab) < 0);
      vector_free (a);
      vector_free (ab);
      vector_free (b);
    }
    {
      VECTOR(long double) a = vector_init ((long double)1, 2, 3, 4, 5);
      VECTOR(long double) b = vector_clone (a);
      su_assert_eq (vector_compare (a, b), 0)
      vector_free (a);
      vector_free (b);
    }
    {
      VECTOR(int) v = vector_init (1, 2, 3);
      su_assert (vector_compare (v, NULL) > 0);
      su_assert (vector_compare (NULL, v) < 0);
      su_assert (vector_compare (NULL, NULL) == 0);
      vector_free (v);
    }
  })

  su_test("vector_push", {
    for (int i = 0; i < ITERATIONS; ++i) {
      vector_push(ivec, i);
    }
    su_assert_eq(vector_size(ivec), ITERATIONS);
    su_assert_eq(vector_capacity(ivec), next_power_of_2(ITERATIONS));
    for (int i = 0; i < ITERATIONS; ++i) {
      su_assert_eq(ivec[i], i);
    }
  })

#ifdef vector_emplace_back
  su_test("vector_emplace_back", {
    struct MyStruct *my_vec = NULL;
    vector_emplace_back(my_vec, .i = 1, .f = 1.0, .s = "one");
    vector_emplace_back(my_vec, .f = 2.0, "two", .i = 2);
    vector_emplace_back(my_vec, 3, 3.0, "three");
    vector_free(my_vec);
  })
#endif

  su_test ("vector_push_vector", {
    VECTOR(int) v = vector_init (1, 2, 3);
    VECTOR(int) o = vector_init (4, 5, 6);
    vector_push_vector (v, o);
    vector_free (o);
    su_assert (check (v, 6, 1, 2, 3, 4, 5, 6));
    vector_free (v);
  })

  su_test("vector_pop", {
    su_assert_eq(vector_pop(ivec), ITERATIONS - 1);
    for (int i = 0; i < (ITERATIONS/2 - 2); ++i) {
      (void)vector_pop(ivec);
    }
    su_assert_eq(vector_pop(ivec), ITERATIONS - ITERATIONS/2);
    su_assert_eq(vector_size(ivec), ITERATIONS/2);
  })

  su_test("vector_insert", {
    vector_insert(ivec, 8, 30);
    vector_insert(ivec, 4, 20);
    vector_insert(ivec, 2, 10);
    su_assert_eq(vector_size(ivec), ITERATIONS/2 + 3);
    su_assert_eq(ivec[10], 30);
    su_assert_eq(ivec[5], 20);
    su_assert_eq(ivec[2], 10);

    size_t index = vector_size(ivec);
    vector_insert(ivec, index, 123);
    su_assert_eq(ivec[index], 123);

    int *empty = NULL;
    vector_insert(empty, 0, 1);
    su_assert_eq(vector_size(empty), 1);
    su_assert_eq(empty[0], 1);
  })

  su_test("vector_remove", {
    vector_remove(ivec, 9);
    vector_remove(ivec, 4);
    vector_remove(ivec, 1);
    su_assert_eq(ivec[7], 30);
    su_assert_eq(ivec[3], 20);
    su_assert_eq(ivec[1], 10);
  })

  su_test ("vector_insert", {
    size_t before = vector_size (ivec);
    vector_insert (ivec, 5, 55);
    vector_insert (ivec, 7, 77);
    su_assert_eq (before+2, vector_size (ivec));
    su_assert_eq (ivec[5] ,55);
    su_assert_eq (ivec[7], 77);
  })

#ifdef vector_emplace
  su_test ("vector_emplace", {
    struct MyStruct *my_vec = NULL;
    vector_emplace_back (my_vec, 1, 1.0, "one");
    vector_emplace_back (my_vec, 1, 1.0, "one");
    vector_emplace_back (my_vec, 1, 1.0, "one");
    vector_emplace (my_vec, 1, 2, 2.0, "two");
    vector_emplace (my_vec, 3, .i=3, .f=3.0, .s="three");
    su_assert_eq (vector_size (my_vec), 5);
    su_assert_eq (my_vec[1].i, 2);
    su_assert_eq (my_vec[3].i, 3);
    vector_free (my_vec);
  })
#endif

  su_test ("vector_erase", {
    size_t before = vector_size (ivec);
    vector_erase (ivec, 4, 5);
    su_assert_eq (before-5, vector_size (ivec));
    su_assert_eq (ivec[4], 30);

    VECTOR(int) v = vector_init (1, 2, 3, 4, 5);
    vector_erase (v, vector_size (v) - 3, 3);
    su_assert (check (v, 2, 1, 2));
    vector_free (v);
  })

  su_test("vector_shrink_to_fit", {
    vector_shrink_to_fit(ivec);
    su_assert_eq(vector_size(ivec), vector_capacity(ivec));
  })

  su_test ("vector_clear", {
    size_t before = vector_capacity (ivec);
    vector_clear (ivec);
    su_assert_eq (vector_size (ivec), 0);
    su_assert_eq (vector_capacity (ivec), before);
  })

  su_test ("vector_resize", {
    vector_resize (ivec, 40);
    ivec[39] = 0xabc;
    su_assert_eq (ivec[39], 0xabc);
  })

#ifdef vector_init
  su_test ("vector_init", {
    int *v = vector_init (1, 2, 3, 4, 5, 6);
    su_assert (vector_capacity (v) >= 6);
    su_assert (check (v, 6, 1, 2, 3, 4, 5, 6));
    vector_free (v);
  })
#endif

  su_test ("vector_clone", {
    int *my_vec = vector_clone (ivec);
    su_assert_eq (vector_size (my_vec), vector_size (ivec));
    for (size_t i = 0; i < vector_size (my_vec); ++i)
      su_assert_eq (my_vec[i], ivec[i]);
    vector_free (my_vec);
  })

  su_test ("vector_copy", {
    int *v1 = NULL;
    int *v2 = vector_create (int, vector_size (ivec) / 2);
    int *v3 = vector_create (int, vector_size (ivec) + 5);
    vector_copy (v1, ivec);
    vector_copy (v2, ivec);
    vector_copy (v3, ivec);
    su_assert_eq (vector_size (v1), vector_size (ivec));
    su_assert_eq (vector_size (v2), vector_size (ivec));
    su_assert_eq (vector_size (v3), vector_size (ivec));
    for (size_t i = 0; i < vector_size (ivec); ++i)
      {
        su_assert_eq (v1[i], ivec[i]);
        su_assert_eq (v2[i], ivec[i]);
        su_assert_eq (v3[i], ivec[i]);
      }
    vector_free (v1);
    vector_free (v2);
    vector_free (v3);
    int *v4 = NULL;
    int *v5 = vector_init (4, 5, 6);
    vector_copy (v5, v4);
    su_assert (v5);
    su_assert_eq (vector_size (v5), 0);
    vector_free (v5);
  })

  su_test ("vector_for_each",  {
    int *my_vec = vector_init (1, 2, 3, 4, 5);
    int i = 0;
    vector_for_each (my_vec, elem)
      su_assert_eq (*elem, ++i);
    vector_free (my_vec);
    i = 0;
    int *none = NULL;
    vector_for_each (none, it)
      ++i;
    su_assert_eq (i, 0);
  })

  su_test ("vector_slice", {
    VECTOR(int) v = vector_create_from (G_int_buffer, 10);

    VECTOR(int) first_five = vector_slice (v, 0, 5);
    su_assert (check (first_five, 5, 0, 1, 2, 3, 4));
    vector_free (first_five);

    VECTOR(int) first_five_2 = vector_slice (v, -10, -5);
    su_assert (check (first_five_2, 5, 0, 1, 2, 3, 4));
    vector_free (first_five_2);

    VECTOR(int) last_five = vector_slice (v, -5, 10);
    su_assert (check (last_five, 5, 5, 6, 7, 8, 9));
    vector_free (last_five);

    VECTOR(int) end_out_of_bounds = vector_slice (v, 8, 100);
    su_assert (check (end_out_of_bounds, 2, 8, 9));
    vector_free (end_out_of_bounds);

    VECTOR(int) begin_out_of_bounds = vector_slice (v, 20, 30);
    su_assert_eq (begin_out_of_bounds, NULL);
    vector_free (begin_out_of_bounds);

    VECTOR(int) begin_after_end = vector_slice (v, -3, 4);
    su_assert_eq (begin_after_end, NULL);
    vector_free (begin_after_end);

    VECTOR(int) slice_of_null = vector_slice (NULL, 0, 1);
    su_assert_eq (slice_of_null, NULL);
    vector_free (slice_of_null);

    VECTOR(int) all = vector_slice (v, 0, VECTOR_SLICE_REST);
    su_assert (check (all, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    vector_free (all);

    vector_free (v);
  })

  su_test ("vector_select", {
    VECTOR(int) v = vector_init (1, 2, 3, 4, 5);

    VECTOR(int) r = vector_select (v, 4, 3, 2, 1, 0);
    su_assert (check (r, 5, 5, 4, 3, 2, 1));
    vector_free (r);

    VECTOR(int) r2 = vector_select (v, -1, -2, -3, -4, -5);
    su_assert (check (r2, 5, 5, 4, 3, 2, 1));
    vector_free (r2);

    VECTOR(int) four = vector_select (v, 3, 3, 3, 3);
    su_assert (check (four, 4, 4, 4, 4, 4));
    vector_free (four);

    vector_free (v);

    VECTOR(int) empty = vector_select ((int *)NULL, 1, 2, 3);
    su_assert_eq (empty, NULL);
  })

  vector_free(ivec);
})

su_module (static_vector_tests, {
  su_test ("vector_create_static", {
    int buf[VECTOR_STATIC_SIZE (int, 10)];
    VECTOR(int) v = vector_create_static (buf);
    su_assert_eq (vector_size (v), 0);
    su_assert_eq (vector_capacity (v), 10);
    su_assert_eq (vector_static_size (v), VECTOR_STATIC_SIZE (int, 10));
  })

  su_test ("vector_create_static_sized", {
    int buf[VECTOR_STATIC_SIZE (int, 20)];
    int *buf_ptr = buf;
    VECTOR(int) v = vector_create_static_sized (buf_ptr, 10);
    su_assert_eq (vector_size (v), 0);
    su_assert_eq (vector_capacity (v), 10);
    su_assert_eq (vector_static_size (v), VECTOR_STATIC_SIZE (int, 10));
  })

  su_test ("multiple in same buffer", {
    int buf[VECTOR_STATIC_SIZE (int, 10) * 10];
    int *buf_ptr = buf;
    int *vectors[10];
    for (int i = 0; i < 10; ++i) {
      vectors[i] = vector_create_static_sized (buf_ptr, 10);
      buf_ptr += VECTOR_STATIC_SIZE (int, 10);
    }
    for (int i = 0; i < 10; ++i) {
      su_assert_eq (vector_size (vectors[i]), 0);
      su_assert_eq (vector_capacity (vectors[i]), 10);
    }
  })

  su_test ("operations", {
    int buf[VECTOR_STATIC_SIZE (int, 10)];
    VECTOR(int) v = vector_create_static (buf);
    for (int i = 0; i < 10; ++i) {
      vector_push (v, i);
    }
    su_assert (check (v, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
    for (int i = 0; i < 5; ++i) {
      (void)vector_pop (v);
    }
    su_assert (check (v, 5, 0, 1, 2, 3, 4));
    for (int i = 0; i < 3; ++i) {
      vector_insert (v, 3+i, 20+i);
    }
    su_assert (check (v, 8, 0, 1, 2, 20, 21, 22, 3, 4));
    vector_erase (v, 4, 3);
    su_assert (check (v, 5, 0, 1, 2, 20, 4));
    vector_clear (v);
    su_assert (vector_empty (v));
  })
})

int main() {
  su_run_module(vector_tests);
  su_run_module(static_vector_tests);
}

