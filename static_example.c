#include <assert.h>
#define VECTOR_IMPLEMENTATION
#include "static_vector.h"

struct Big_Type {
    size_t a;
    size_t b;
};

int main(void) {
    enum {
        CAPACITY = 10,
        SMALL = 3
    };
    struct Big_Type buf[VECTOR_STATIC_SIZE(struct Big_Type, CAPACITY)];
    struct Big_Type *vec = vector_create_static(buf);
    assert(vector_capacity(vec) == CAPACITY);
    // the header of the vector also holds 2 `size_t`s so it's 1 extra element.
    assert(vector_static_size(vec) == CAPACITY + 1);

    vec = vector_create_static_sized(buf, SMALL);
    assert(vector_capacity(vec) == SMALL);
    assert(vector_static_size(vec) == SMALL + 1);
}
