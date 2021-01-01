#include "aux/vector.h"
#include <stdlib.h>
#include <string.h>

void* aux_vector_extract_data(struct aux_vector *v) {
    void *data = v->av_memory;
    memset(v, 0, sizeof(struct aux_vector));
    return data;
}

static void aux_vector_reallocate(struct aux_vector *v, size_t capacity) {
    if (v->av_capacity != capacity) {
        void *new_memory = realloc(v->av_memory, capacity * v->av_element_size);
        if (new_memory == NULL) {
            abort();
        }
        v->av_memory = new_memory;
        v->av_capacity = capacity;
        if (v->av_length > v->av_capacity) {
            v->av_length = v->av_capacity;
        }
    }
}

void aux_vector_init(struct aux_vector *v, size_t element_size, size_t capacity) {
    v->av_memory = NULL;
    v->av_element_size = element_size;
    v->av_capacity = 0;
    v->av_length = 0;

    if (capacity != 0) {
        aux_vector_reallocate(v, capacity);
    }
}

void aux_vector_release(struct aux_vector *v, aux_vector_free_func_p cb) {
    if (cb) {
        for (size_t i = 0; i < v->av_length; ++i) {
            cb(aux_vector_at(v, i));
        }
    }

    free(v->av_memory);
    v->av_memory = NULL;
}

void aux_vector_clear(struct aux_vector *v, aux_vector_free_func_p cb) {
    for (size_t i = 0; i < v->av_length; ++i) {
        cb(aux_vector_at(v, i));
    }
    v->av_length = 0;
}

size_t aux_vector_reserve(struct aux_vector *v, size_t amount) {
    if (v->av_length + amount > v->av_capacity) {
        aux_vector_reallocate(v, v->av_length + amount);
    }

    return v->av_capacity;
}

void aux_vector_walk(struct aux_vector *v, bool (*walk_fn)(void *, void *), void *user) {
    for (size_t i = 0; i < v->av_length; ++i) {
        bool break_early = walk_fn(aux_vector_at(v, i), user);
        if (break_early) {
            break;
        }
    }
}

void *aux_vector_push_back(struct aux_vector *v, void *element, size_t element_size) {
    if (v->av_capacity == 0) {
        aux_vector_reallocate(v, 1);
    } else if (v->av_length == v->av_capacity) {
        aux_vector_reallocate(v, v->av_capacity * 2);
    }

    void *destination = v->av_memory + v->av_length * v->av_element_size;
    size_t bytes_to_copy = element_size > v->av_element_size ? v->av_element_size : element_size;
    memcpy(destination, element, bytes_to_copy);
    v->av_length += 1;
    return destination;
}

void *aux_vector_at(struct aux_vector *v, size_t index) {
    if (index < v->av_length) {
        return v->av_memory + v->av_element_size * index;
    }
    return NULL;
}

size_t aux_vector_length(struct aux_vector const *v) {
    return v->av_length;
}
