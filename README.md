# c-vector

A dynamic array based on [stb stretchy buffer](https://github.com/nothings/stb/blob/master/stb_ds.h) (see Acknowledgments section).

Adds some more functionality known from c++ `std::vector`, mainly the ability to insert/remove elements at any index.

## Documentation

### Example

```c
#include <stdio.h>
#include "vector.h"

int
main ()
{
  int *v = vector_init (7, 5, 16, 8);

  vector_push (v, 25);
  vector_push (v, 13);

  printf ("v = { ");
  for (size_t i = 0; i < vector_size (v); ++i)
    printf ("%d, ", v[i]);
  printf ("}\n");

  vector_free (v);
}
```

Output:

```
v = { 7, 5, 16, 8, 25, 13, }
```

### Types

```c
#define VECTOR(T) T *
```

This is used to be clear about something being a vector and not just a pointer in source code.

### Synopsis

```c
/**
 * Parameters:
 *    v - vector
 *    e - element
 *    n - number of elements
 *    i - index of element
 *    T - type
 *  ... - initializer list elements
 */

/* Gets the number of elements in the vector. */
#define vector_size(v)

/* Gets the number of elements that fit in the vector. */
#define vector_capacity(v)

/* Checks if the vector is empty. */
#define vector_empty(v)

/* If I is negative `vector_size (v) - i`, otherwise just unchanged I. */
#define vector_idx(v, i)

/* Checks if I is a valid index (see vector_idx) */
#define vector_idx_valid(v, i)

/* Gets a reference to the element at index I with bounds checking.
   If I is a negative number, gets the element at `vector_size(v) - i`.
   If I is out of bounds the result is NULL. */
#define vector_at(v, i)

/* Increases the capacity of the vector the fit at least N elements. */
#define vector_reserve(v, n)

/* Reduces the vectors capacity to its size. */
#define vector_shrink_to_fit(v)

/* Gets the last element of the vector. */
#define vector_back(v)

/* Gets a pointer past the last element of the vector. */
#define vector_end(v)

/* Appends an element to the vector. */
#define vector_push(v, e)

/* For vectors of structures or unions, appends a new element to the vector and
   constructs it in-place, variadic arguments are passed as initializer list
   for struct or union. */
#define vector_emplace_back(v, ...)

/* Gets and removes the last element of the vector. */
#define vector_pop(v)

/* Inserts a new element into the vector at position I. */
#define vector_insert(v, i, e)

/* Like vector_emplace_back but the new element is inserted at position I. */
#define vector_emplace(v, i, ...)

/* Removes the element at position I from the vector. */
#define vector_remove(v, i)

/* Removes N elements from the vector, starting at position I. */
#define vector_erase(v, i, n)

/* Clears the contents of the vector. */
#define vector_clear(v)

/* Resizes the vector. */
#define vector_resize(v, n)

/* Creates a new empty vector. */
#define vector_create(T, n)

/* Creates a new vector with elements {X, ...} and the type of X as element
   type. */
#define vector_init(x, ...)

/* Creates a vector with the first N elements form the buffer pointed to by P.
 */
#define vector_create_from(p, n)

/* Frees the vector. */
#define vector_free(v)

/* Create a new vector with the same elements as the input vector */
#define vector_copy_construct(v)

/* Copy data from SRC to DST */
#define vector_copy(dst, src)

/* Iterate over the vector, IT recieves a pointer to each element */
#define vector_for_each(v, it)

/* Creates a vector from `v[b:e]`. B and E may be negative (see vector_idx).
   If B or E are out of bounds they get clamped into the valid range. */
#define vector_slice(v, b, e)
```

### Null pointer safety

Most of these may be called with `v` being a null pointer, in this case they will either

- Return `0`/`NULL`: `vector_size`, `vector_capacity`, `vector_end`, `vector_copy_construct`, `vector_idx_valid`, `vector_at`, `vector_slice`

- Return `1`: `vector_empty`

- Do nothing: `vector_shrink_to_fit`, `vector_insert`, `vector_emplace`, `vector_remove`, `vector_erase`, `vector_clear`, `vector_free`

- Create a new vector: `vector_reserve`, `vector_push`, `vector_emplace_back`, `vector_copy`

The only exceptions are `vector_back` and `vector_pop` which will cause a segmentation fault.

`vector_idx (NULL, -n)` will result in `(size_t)-n` (overflows into a huge number).

### Availability

By default `vector_emplace_back`, `vector_emplace`, `vector_for_each` and `vector_init` are only defined if `__GNUC__` is defined.

`vector_emplace_back`, `vector_emplace` and `vector_for_each` can be enabled by defining `VECTOR__DECLTYPE(x)`, which gives the type of `x`.

If your compiler does support [statement expressions](https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html), `vector_init` can be enabled by defining `VECTOR__HAS_STATEMENT_EXPRS`.

## static vectors

`static_vector.h` contains utilities to create vectors with a static capacity inside existing buffers.

### Synopsis

```c
/* Number of elements of type T required to hold the header of a vector. */
#define VECTOR__HEAD_SPACE(T)

/* Number of elements an array of T needs to hold a vector of T with capacity N. */
#define VECTOR_STATIC_SIZE(T, n)

/* Creates a vector in the static array V. */
#define vector_create_static(v)

/* Creates a static vector with a capacity of N in the buffer pointer to by V. */
#define vector_create_static_sized(v, n)

/* Number of elements the static vector occupies (including its header). */
#define vector_static_size(v)
```

### Usage

Static vectors use the same functions as normal vectors.
There are no checks whether a operation exceeds the static capacity of the vector, instead these operations will try to reallocate and crash.

## Acknowledgments

Based on an old version of stb, its implementation has since evolved quite a lot (and is no longer even named stretchy buffer).

