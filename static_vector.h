#ifndef STATIC_VECTOR_H
#define STATIC_VECTOR_H
#include "vector.h"

/* Number of elements of type T required to hold the header of a vector. */
#define VECTOR__HEAD_SPACE(T)                                          \
  (sizeof (T) >= sizeof (struct vector__header)                        \
   ? 1                                                                 \
   : ((sizeof (struct vector__header) + sizeof (T) - 1) / sizeof (T)))

/* Number of elements an array of T needs to hold a vector of T with capacity N. */
#define VECTOR_STATIC_SIZE(T, n)\
  (VECTOR__HEAD_SPACE (T) + (n))

/* Creates a vector in the static array V. */
#define vector_create_static(v)                    \
  (VECTOR__DECLTYPE (*v) *)vector__create_static ( \
     (void *)(v),                                  \
     sizeof (*v),                                  \
     sizeof (v),                                   \
     VECTOR__HEAD_SPACE (VECTOR__DECLTYPE (*v))    \
  )

/* Creates a static vector with a capacity of N in the buffer pointer to by V. */
#define vector_create_static_sized(v, n)                            \
  (VECTOR__DECLTYPE (*v) *)vector__create_static (                  \
    (void *)(v),                                                    \
    sizeof (*v),                                                    \
    (n + VECTOR__HEAD_SPACE (VECTOR__DECLTYPE (*v))) * sizeof (*v), \
    VECTOR__HEAD_SPACE (VECTOR__DECLTYPE (*v))                      \
  )

/* Number of elements the static vector occupies (including its header). */
#define vector_static_size(v)\
  (VECTOR__HEAD_SPACE (VECTOR__DECLTYPE (*v)) + vector__capacity (v))



VECTOR__MAYBE_UNUSED static void *
vector__create_static (void *buf, size_t elem_size, size_t buf_size, size_t head_space)
{
  struct vector__header *head = (struct vector__header *)((char*)buf + head_space);
  head->size = 0;
  head->capacity = (buf_size - (head_space * elem_size)) / elem_size;
  return (void *)head->data;
}

#endif

