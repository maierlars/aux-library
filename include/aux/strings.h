#ifndef AUX_STRINGS
#define AUX_STRINGS
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct aux_string       aux_string_t, *aux_string_p;
typedef struct aux_string_slice aux_string_slice_t;

uint8_t        aux_string_at (aux_string_p str, size_t idx);
const uint8_t *aux_string_get_buffer (aux_string_p str);
const char *   aux_string_as_c_string (aux_string_p str);
size_t         aux_string_length (aux_string_p str);
aux_string_p   aux_string_alloc (size_t size);
aux_string_p   aux_string_from_buffer (const void *b, size_t size);
aux_string_p   aux_string_from_c_string (const char *b);
aux_string_p   aux_string_from_slice (aux_string_slice_t slice);
void           aux_string_free (aux_string_p str);
aux_string_p   aux_string_copy (aux_string_p str);
aux_string_p   aux_string_dup (aux_string_p str);
aux_string_p   _aux_string_concat (aux_string_p a, aux_string_p b);

#define aux_string_concat(...) _aux_string_concat_multi (VA_NARG (__VA_ARGS__), __VA_ARGS__)
aux_string_p _aux_string_concat_multi (size_t n, ...);

uint64_t aux_string_hash (aux_string_p a);
uint64_t aux_string_slice_hash (aux_string_slice_t a);
size_t   aux_string_find (aux_string_p haystack, aux_string_p needle);
size_t   aux_string_find_next (aux_string_p haystack, aux_string_p needle, size_t offset);

int  aux_string_cmp (aux_string_p a, aux_string_p b);
bool aux_string_equals (aux_string_p a, aux_string_p b);
bool aux_string_equals_slice (aux_string_p a, aux_string_slice_t b);
bool aux_string_slice_equals_c_string (aux_string_slice_t a, const char *b);
bool aux_string_equals_c_string (aux_string_p a, const char *b);

bool aux_string_starts_with (aux_string_p str, aux_string_p prefix);

typedef struct aux_string_split_iter aux_string_split_iter_t;

void aux_string_split_string (aux_string_p str, aux_string_p delim, aux_string_split_iter_t *iter);
bool aux_string_split_iter_next (aux_string_split_iter_t *iter, aux_string_slice_t *slice);
void aux_string_split_iter_release (aux_string_split_iter_t *iter);

aux_string_slice_t aux_string_slice_all (aux_string_p string);
aux_string_slice_t aux_string_slice_from (aux_string_p string, size_t offset);
aux_string_slice_t aux_string_slice_to (aux_string_p string, size_t to);
aux_string_slice_t aux_string_slice_from_to (aux_string_p string, size_t from, size_t to);

size_t  aux_string_slice_length (aux_string_slice_t slice);
uint8_t aux_string_slice_at (aux_string_slice_t slice, size_t at);

struct aux_string_split_iter {
    aux_string_p string;
    aux_string_p delim;
    size_t       current;
};

struct aux_string {
    size_t        length;
    atomic_size_t ref;
    uint8_t       buffer[];
};

struct aux_string_slice {
    aux_string_p string;
    size_t       from, to;  // [from, to)
};

#endif
