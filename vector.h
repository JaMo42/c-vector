/* Based on stb stretchy buffer
 * https://github.com/nothings/stb/blob/master/stretchy_buffer.h
 */
#ifndef VECTOR_H
#define VECTOR_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct vector__header {
  size_t size;
  size_t capacity;
  char data[];
};

#ifdef __cplusplus
# define vector__decltype(x) decltype(x)
# define VECTOR__HAS_DECLTYPE
#else // __cplusplus
# ifdef __GNUC__
#  ifdef __clang__
#   define vector__decltype(x) __typeof__(x)
#  else // __clang__
#   define vector__decltype(x) typeof(x)
#  endif // __clang__
#  define VECTOR__HAS_DECLTYPE
# endif // __GNUC__
#endif // __cplusplus

/**
 * Parameters:
 *    v - vector
 *    e - element
 *    n - number of elements
 *    i - index of element
 *    T - type
 *  ... - initializer list elements
 */

#define vector__raw(v) ((size_t *)(v) - 2)
#define vector__get(v) ((struct vector__header *)vector__raw(v))
#define vector__size(v) (vector__get(v)->size)
#define vector__capacity(v) (vector__get(v)->capacity)

#define vector__grow(v, n) (*((void **)&(v)) = vector__grow_f((v), (n), sizeof(*(v))))
#define vector__needgrow(v, n) ((v) == NULL || vector__size(v) + (n) >= vector__capacity(v))
#define vector__maybegrow(v, n) (vector__needgrow((v), (n)) ? vector__grow((v), (n)) : 0)

/* Gets the number of elements in the vector. */
#define vector_size(v)\
  ((v) == NULL ? 0 : vector__size(v))

/* Gets the number of elements that fit in the vector. */
#define vector_capacity(v)\
  ((v) == NULL ? 0 : vector__capacity(v))

/* Checks if the vector is empty. */
#define vector_empty(v)\
  ((v) == NULL ? 1 : vector__size(v) == 0)

/* Resizes the vector the fit atleast `n` elements */
#define vector_reserve(v, n)\
  ((n) > vector_capacity(v) ? vector_resize((v), (n)) : 0)

/* Resize the vector the its number of elements */
#define vector_shrink_to_fit(v)\
  vector_resize((v), vector_size(v))

/* Gets the last element of the vector. */
#define vector_back(v)\
  ((v)[vector__size(v) - 1])

/* Gets a pointer past the last element of the vector */
#define vector_end(v)\
  ((v) == NULL ? NULL : ((v) + vector__size(v)))

/* Appends an element to the vector. */
#define vector_push(v, e)        \
  (vector__maybegrow((v), 1),    \
   (v)[vector__size(v)++] = (e))

#ifdef VECTOR__HAS_DECLTYPE
/* For vectors of structures or unions, appends a new element to the vector and
   constructs it in-place, variadic arguments are passed as initializer list
   for struct or union. */
#define vector_emplace_back(v, ...)                                 \
  (vector__maybegrow((v), 1),                                       \
   (v)[vector__size(v)++] = (vector__decltype(*v)) { __VA_ARGS__ })
#endif

/* Gets and removes the last element of the vector. */
#define vector_pop(v)\
  ((v)[--vector__size(v)])

/* Inserts a new element into the vector. */
#define vector_insert(v, i, e)                                \
  (((v) == NULL || (size_t)(i) >= vector_size(v))             \
   ? 0                                                        \
   : (vector__maybegrow((v), 1),                              \
      vector__shift((char*)(void*)(v), (i), 1, sizeof(*(v))), \
      (v)[(i)] = (e),                                         \
      ++vector__size(v)))

#ifdef VECTOR__HAS_DECLTYPE
/* Like vector_emplace_back but element is inserted at position I. */
#define vector_emplace(v, i, ...)                                 \
  (((v) == NULL || (size_t)(i) >= (vector__size (v)))             \
   ? 0                                                            \
   : (vector__maybegrow ((v), 1),                                 \
      vector__shift ((char *)(void *)(v), (i), 1, sizeof (*(v))), \
      (v)[(i)] = (vector__decltype (*v)) { __VA_ARGS__ },         \
      ++vector__size (v)))
#endif

/* Removes an element from the vector. */
#define vector_remove(v, i)                                      \
  (((v) == NULL || (size_t)(i) >= vector_size(v))                \
   ? 0                                                           \
   : (vector__shift((char *)(void *)(v), (i+1), -1, sizeof(*(v))), \
      --vector__size(v)))

/* Removes elements from the vector */
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
  (*((void **)&(v)) = vector__resize_f((v), (n), sizeof(*(v))))

/* Creates a new vector. (optional) */
#define vector_create(T, n)\
  ((T *)vector__create((n), sizeof(T)))

/* Frees a vector. */
#define vector_free(v)\
  ((v) ? (free(vector__raw(v)), 0) : 0)


static void *
vector__resize_f(void *data, size_t elems, size_t elem_size) {
  struct vector__header *v = (struct vector__header *)realloc (
    data ? vector__raw (data) : NULL,
    elems*elem_size + sizeof (struct vector__header));
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
      fputs("vector__resize_f: allocation failed\n", stderr);
      exit (1);
    }
}

static void *
vector__grow_f(void *data, size_t size, size_t elem_size) {
  size_t min_needed = vector_size(data) + size;
  size_t default_growth = vector_capacity(data) << 1;
  size_t new_capacity = default_growth > min_needed ? default_growth : min_needed;
  return vector__resize_f(data, new_capacity, elem_size);
}

static void
vector__shift(char *data, size_t index, long diff, size_t elem_size) {
  char *at = data + index * elem_size;
  size_t count = vector__size(data) - index;
  memmove(at + diff * elem_size, at, count * elem_size);
}

static void *
vector__create(size_t capacity, size_t elem_size) {
  size_t *v = (size_t *)malloc(capacity * elem_size + sizeof(struct vector__header));
  v[0] = 0;
  v[1] = capacity;
  return (void *)(v + 2);
}

#endif /* !VECTOR_H */

