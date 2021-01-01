#ifndef AUX_STRING_BUILDER_H
#define AUX_STRING_BUILDER_H
#include "strings.h"
#include <stdbool.h>



typedef struct aux_string_builder aux_string_builder_t;

bool         aux_string_builder_init_capacity (aux_string_builder_t *builder, size_t capacity);
aux_string_p aux_string_builder_finish (aux_string_builder_t *builder);
uint8_t *    aux_string_builder_grow (aux_string_builder_t *builder, size_t delta);
void         aux_string_builder_init (aux_string_builder_t *builder);
void         aux_string_builder_free (aux_string_builder_t *builder);
void         aux_string_builder_append_c (aux_string_builder_t *builder, char c);
void         aux_string_builder_append_c_times (aux_string_builder_t *builder, char c, size_t times);

void aux_string_builder_append_buffer (aux_string_builder_t *builder, const uint8_t *c, size_t length);
void aux_string_builder_append_c_string (aux_string_builder_t *builder, const char *c);
void aux_string_builder_append_c_nstring (aux_string_builder_t *builder, const char *c, size_t max);
void aux_string_builder_append_string (aux_string_builder_t *builder, aux_string_p str);
void aux_string_builder_append_nstring (aux_string_builder_t *builder, aux_string_p str, size_t max);

void aux_string_builder_append_uint (aux_string_builder_t *builder, uint64_t v);
void aux_string_builder_append_int (aux_string_builder_t *builder, int64_t v);
void aux_string_builder_append_double(aux_string_builder_t *builder, double v);
void aux_string_builder_append_bool (aux_string_builder_t *builder, bool v);

struct aux_string_builder {
    aux_string_p result;
    size_t       length;
};

#define FOO_STRING_BUILDER_EMPTY (aux_string_builder_t){ .result = NULL, .length = 0 };
#define FOO_STRING_BUILDER(var) aux_string_builder_t var = FOO_STRING_BUILDER_EMPTY;
#define FOO_STRING_BUILDER_DEFAULT_CAPACITY 512

#define aux_string_builder_append(b, x) \
    _Generic(x, aux_string_p : aux_string_builder_append_string, const char* : aux_string_builder_append_c_string, char *: aux_string_builder_append_c_string, char: aux_string_builder_append_c) (b, x)

#endif
