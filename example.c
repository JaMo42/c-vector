#include <stdio.h>
#define VECTOR_IMPLEMENTATION
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
