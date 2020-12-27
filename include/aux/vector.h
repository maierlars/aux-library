#ifndef AUX_VECTOR_H_
#define AUX_VECTOR_H_
#include <stddef.h>
#include <stdbool.h>

struct aux_vector {
    size_t av_element_size;
    size_t av_capacity;
    size_t av_length;
    void *av_memory;
};

typedef void (*aux_vector_free_func_p)(void *);

void aux_vector_init(struct aux_vector *v, size_t element_size, size_t capacity);

void aux_vector_release(struct aux_vector *v, aux_vector_free_func_p);

void *aux_vector_push_back (struct aux_vector *v, void *element, size_t element_size);
bool aux_vector_pop_back (struct aux_vector *v, void *element, size_t element_size);

void aux_vector_walk(struct aux_vector *v, bool (*walk_fn)(void *element, void *user), void *user);

void aux_vector_clear(struct aux_vector *v, aux_vector_free_func_p);
void *aux_vector_at(struct aux_vector *v, size_t index);

size_t aux_vector_length(const struct aux_vector *v);
size_t aux_vector_capacity(const struct aux_vector *v);
size_t aux_vector_element_size(const struct aux_vector *v);

size_t aux_vector_reserve(struct aux_vector *v, size_t amount);
size_t aux_vector_shrink_to_fit(struct aux_vector *v);
size_t aux_vector_shrink_to(struct aux_vector *v, size_t limit, aux_vector_free_func_p);

#endif  // AUX_VECTOR_H_
