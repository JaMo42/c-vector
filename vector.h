/* Based on stb stretchy buffer
 * https://github.com/nothings/stb/blob/master/stb_ds.h
 */
#ifndef VECTOR_H
#define VECTOR_H
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct vector__header {
  size_t size;
  size_t capacity;
  char data[];
};

#ifndef VECTOR__DECLTYPE
# ifdef __cplusplus
#  define VECTOR__DECLTYPE(x) decltype(x)
# else /* __cplusplus */
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__)
#   define VECTOR__DECLTYPE(x) typeof(x)
#  else
#   define VECTOR__DECLTYPE(x) __typeof__(x)
#  endif /* __GNUC__ */
# endif /* __cplusplus */
#endif /* VECTOR__DECLTYPE */

#ifndef VECTOR__MAYBE_UNSUED
# ifdef __cplusplus
#  define VECTOR__MAYBE_UNUSED [[maybe_unused]]
# else /* __cplusplus */
#  ifdef __GNUC__
#   define VECTOR__MAYBE_UNUSED __attribute__((unused))
#  else /* __GNUC__ */
#   define VECTOR__MAYBE_UNUSED
#  endif /* __GNUC__ */
# endif /* __cplusplus */
#endif /* VECTOR__MAYBE_UNUSED */

#ifndef VECTOR__HAS_STATEMENT_EXPRS
# ifdef __GNUC__
#  define VECTOR__HAS_STATEMENT_EXPRS
# endif /* __GNUC__ */
#endif /* VECTOR__HAS_STATEMENT_EXPRS */

/**
 * Parameters:
 *    v - vector
 *    e - element
 *    n - number of elements
 *    i - index of element
 *    T - type
 *  ... - initializer list elements
 */

#define vector__get(v) (((struct vector__header *)(v)) - 1)
#define vector__size(v) (vector__get(v)->size)
#define vector__capacity(v) (vector__get(v)->capacity)

/* Grow the vector so it can fit at least N more items. */
#define vector__grow(v, n) (*((void **)&(v)) = vector__grow_impl((v), (n), sizeof(*(v))))
/* Check if the vector needs to grow to accommodate N more items. */
#define vector__needgrow(v, n) ((v) == NULL || vector__size(v) + (n) >= vector__capacity(v))
/* Ensure that the vector can fit N more items, grow it if necessary. */
#define vector__maybegrow(v, n) (vector__needgrow((v), (n)) ? vector__grow((v), (n)) : 0)

/* Same as `T *`, to better document source code. */
#define VECTOR(T) T *

/* Gets the number of elements in the vector. */
#define vector_size(v)\
  ((v) == NULL ? 0 : vector__size(v))

/* Gets the number of elements that fit in the vector. */
#define vector_capacity(v)\
  ((v) == NULL ? 0 : vector__capacity(v))

/* Checks if the vector is empty. */
#define vector_empty(v)\
  ((v) == NULL ? 1 : vector__size(v) == 0)

/* If I is negative `vector_size (v) - i`, otherwise just unchanged I. */
#define vector_idx(v, i)   \
  ((i) < 0                 \
   ? vector_size (v) + (i) \
   : (size_t)(i))

/* Checks if I is a valid index (see vector_idx) */
#define vector_idx_valid(v, i)\
  ((v) && vector_idx ((v), (i)) < vector__size (v))

/* Gets a reference to the element at index I with bounds checking.
   If I is a negative number, gets the element at `vector_size(v) - i`.
   If I is out of bounds the result is NULL. */
#define vector_at(v, i)          \
  (vector_idx_valid ((v), (i))   \
   ? &(v)[vector_idx ((v), (i))] \
   : NULL)

/* Increases the capacity of the vector the fit at least N elements. */
#define vector_reserve(v, n)\
  ((n) > vector_capacity(v) ? vector_resize((v), (n)) : 0)

/* Reduces the vectors capacity to its size. */
#define vector_shrink_to_fit(v)\
  ((v) == NULL ? NULL : vector_resize((v), vector_size(v)))

/* Gets the last element of the vector. */
#define vector_back(v)\
  ((v)[vector__size(v) - 1])

/* Gets a pointer past the last element of the vector. */
#define vector_end(v)\
  ((v) == NULL ? NULL : ((v) + vector__size(v)))

/* Appends an element to the vector. */
#define vector_push(v, e)        \
  (vector__maybegrow((v), 1),    \
   (v)[vector__size(v)++] = (e))

#ifdef VECTOR__DECLTYPE
/* For vectors of structures or unions, appends a new element to the vector and
   constructs it in-place, variadic arguments are passed as initializer list
   for struct or union. */
#define vector_emplace_back(v, ...)                                 \
  (vector__maybegrow((v), 1),                                       \
   (v)[vector__size(v)++] = (VECTOR__DECLTYPE(*v)) { __VA_ARGS__ })
#endif

/* Gets and removes the last element of the vector. */
#define vector_pop(v)\
  ((v)[--vector__size(v)])

/* Inserts a new element into the vector at position I. */
#define vector_insert(v, i, e)                                \
  (((v) == NULL || (size_t)(i) >= vector_size(v))             \
   ? 0                                                        \
   : (vector__maybegrow((v), 1),                              \
      vector__shift((char*)(void*)(v), (i), 1, sizeof(*(v))), \
      (v)[(i)] = (e),                                         \
      ++vector__size(v)))

#ifdef VECTOR__DECLTYPE
/* Like vector_emplace_back but the new element is inserted at position I. */
#define vector_emplace(v, i, ...)                                 \
  (((v) == NULL || (size_t)(i) >= (vector__size (v)))             \
   ? 0                                                            \
   : (vector__maybegrow ((v), 1),                                 \
      vector__shift ((char *)(void *)(v), (i), 1, sizeof (*(v))), \
      (v)[(i)] = (VECTOR__DECLTYPE (*v)) { __VA_ARGS__ },         \
      ++vector__size (v)))
#endif

/* Removes the element at position I from the vector. */
#define vector_remove(v, i)                                        \
  (((v) == NULL || (size_t)(i) >= vector_size(v))                  \
   ? 0                                                             \
   : (vector__shift((char *)(void *)(v), (i+1), -1, sizeof(*(v))), \
      --vector__size(v)))

/* Removes N elements from the vector, starting at position I. */
#define vector_erase(v, i, n)                                               \
  (((v) == NULL || (size_t)(i) >= (vector__size (v) - (n)))                 \
   ? 0                                                                      \
   : (vector__shift ((char *)(void *)(v), (i)+(n), 0LL-(n), sizeof (*(v))), \
      vector__size (v) -= (n)))

/* Clears the contents of the vector. */
#define vector_clear(v)\
  ((v) == NULL ? 0 : (vector__size (v) = 0))

/* Resizes the vector. */
#define vector_resize(v, n)\
  (*((void **)&(v)) = vector__resize_impl((v), (n), sizeof(*(v))))

/* Creates a new empty vector. */
#define vector_create(T, n)\
  ((T *)vector__create((n), sizeof(T)))

#ifdef VECTOR__HAS_STATEMENT_EXPRS
/* Creates a new vector with elements {X, ...} and the type of X as element
   type. */
#define vector_init(x, ...)                                                \
  ({                                                                       \
    VECTOR__DECLTYPE (x) __t[] = { x __VA_OPT__ (,) __VA_ARGS__ };         \
    size_t __l = sizeof (__t) / sizeof (x);                                \
    VECTOR__DECLTYPE (x) *__v = vector_create (VECTOR__DECLTYPE (x), __l); \
    memcpy (__v, __t, __l * sizeof (x));                                   \
    vector__size (__v) = __l;                                              \
    __v;                                                                   \
  })
#endif

/* Creates a vector with the first N elements form the buffer pointed to by P.
 */
#define vector_create_from(p, n)                              \
  memcpy (vector__create_with_size ((n), sizeof (*(p)), (n)), \
          (p),                                                \
          (n) * sizeof(*(p)))

/* Frees the vector. */
#define vector_free(v)\
  ((v) ? (free(vector__get(v)), 0) : 0)

/* Create a new vector with the same elements as the input vector */
#define vector_copy_construct(v)                                  \
  ((v)                                                            \
   ? vector__copy (vector__get (vector__create (vector__size (v), \
                                                sizeof (*v))),    \
                   vector__get (v),                               \
                   sizeof (*v))                                   \
   : NULL)

/* Copy data from SRC to DST */
#define vector_copy(dst, src)                                      \
  (dst = (dst                                                      \
          ? (src                                                   \
             ? vector__copy (vector__get (dst), vector__get (src), \
                             sizeof (*dst))                        \
             : ((void)vector_clear (dst), dst))                    \
          : vector_copy_construct (src)))

#ifdef VECTOR__DECLTYPE
/* Iterate over the vector, IT recieves a pointer to each element */
#define vector_for_each(v, it)                                        \
  for (VECTOR__DECLTYPE (v) it = (v), __end = (v) + vector__size (v); \
       it != __end; ++it)
#endif

/* Creates a vector from `v[b:e]`. B and E may be negative (see vector_idx).
   If B or E are out of bounds they get clamped into the valid range. */
#define vector_slice(v, b, e)\
  vector__slice ((const void *)(v), sizeof (*(v)), vector_size(v), (b), (e))



static void *
vector__resize_impl(void *data, size_t elems, size_t elem_size) {
  struct vector__header *v = (struct vector__header *)realloc (
    data ? vector__get (data) : NULL,
    elems * elem_size + sizeof (struct vector__header));
  if (v)
    {
      if (!data)
        v->size = 0;
      else if (elems < v->size)
        v->size = elems;
      v->capacity = elems;
      return v->data;
    }
  else
    {
      fputs ("vector__resize_impl: allocation failed\n", stderr);
      exit (1);
    }
}

VECTOR__MAYBE_UNUSED static void *
vector__grow_impl(void *data, size_t size, size_t elem_size) {
  size_t min_needed = vector_size (data) + size;
  size_t default_growth = vector_capacity (data) << 1;
  size_t new_capacity = (default_growth > min_needed
                         ? default_growth
                         : min_needed);
  return vector__resize_impl (data, new_capacity, elem_size);
}

VECTOR__MAYBE_UNUSED static void
vector__shift(char *data, size_t index, long diff, size_t elem_size) {
  char *at = data + index * elem_size;
  size_t count = vector__size (data) - index;
  memmove (at + diff * elem_size, at, count * elem_size);
}

VECTOR__MAYBE_UNUSED static void *
vector__create(size_t capacity, size_t elem_size) {
  struct vector__header *v = (struct vector__header *)malloc(
    capacity * elem_size + sizeof (struct vector__header));
  v->size = 0;
  v->capacity = capacity;
  return (void *)v->data;
}

VECTOR__MAYBE_UNUSED static void *
vector__create_with_size (size_t capacity, size_t elem_size, size_t size) {
  struct vector__header *v = (struct vector__header *)malloc(
    capacity * elem_size + sizeof (struct vector__header));
  v->size = size;
  v->capacity = capacity;
  return (void *)v->data;
}

VECTOR__MAYBE_UNUSED static void *
vector__copy (struct vector__header *dest, struct vector__header *source,
              size_t elem_size)
{
  if (source->size > dest->capacity)
    dest = vector__get (vector__resize_impl (dest->data, source->size,
                                             elem_size));
  dest->size = source->size;
  return memcpy (dest->data, source->data, source->size * elem_size);
}

VECTOR__MAYBE_UNUSED static void *
vector__slice (const void *data, size_t elem_size, size_t size,
               ptrdiff_t begin, ptrdiff_t end)
{
  const ptrdiff_t ssize = (ptrdiff_t)size;
  if (!data)
    return NULL;
  if (begin < 0)
    {
      if (-begin >= ssize)
        return NULL;
      begin = ssize + begin;
    }
  else if (begin >= ssize)
    return NULL;
  if (end < 0)
    {
      if (-end > ssize)
        end = ssize;
      else
        end = ssize + end;
    }
  else if (end > ssize)
    end = ssize;
  if (begin >= end)
    return NULL;
  const size_t len = end - begin;
  return memcpy (vector__create_with_size (len, elem_size, len),
                 data + begin * elem_size,
                 len * elem_size);
}

#endif /* !VECTOR_H */
