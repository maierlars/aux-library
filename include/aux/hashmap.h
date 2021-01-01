#ifndef AUX_HASHMAP_H
#define AUX_HASHMAP_H
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <stdint.h>   // uintptr_t

typedef uintptr_t aux_hashmap_ptr;
typedef size_t aux_hash_value;

typedef struct aux_hashmap_bucket {
    aux_hashmap_ptr ahb_key;
    aux_hashmap_ptr ahb_value;
} aux_hashmap_bucket;

typedef struct aux_hashmap_methods {
    void (*ahm_free_key)(aux_hashmap_ptr);
    void (*ahm_free_value)(aux_hashmap_ptr);

    int (*ahm_compare_key)(aux_hashmap_ptr, aux_hashmap_ptr);
    aux_hash_value (*ahm_hash_key)(aux_hashmap_ptr);
} aux_hashmap_methods;

typedef struct aux_hashmap {
    size_t ah_capacity, ah_load;
    aux_hashmap_bucket *ah_buckets;

    aux_hashmap_methods const *ah_methods;
} aux_hashmap;

void aux_hashmap_init(aux_hashmap *h, aux_hashmap_methods const *methods);
void aux_hashmap_release(aux_hashmap *h);

bool aux_hashmap_set(aux_hashmap *h, aux_hashmap_ptr key, aux_hashmap_ptr value, aux_hashmap_ptr *old_value);
bool aux_hashmap_get(aux_hashmap *h, aux_hashmap_ptr key, aux_hashmap_ptr *out);
bool aux_hashmap_delete(aux_hashmap *h, aux_hashmap_ptr key, aux_hashmap_ptr *old_value);

extern aux_hashmap_methods const aux_hashmap_default_methods;

#ifndef NDEBUG
void aux_print_hashmap(aux_hashmap const *h);
#endif

#endif  // AUX_HASHMAP_H
