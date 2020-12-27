#include <assert.h> // assert
#include <stdlib.h> // malloc, free

#include "aux/hashmap.h"

static void aux_hashmap_free_key(aux_hashmap *h, aux_hashmap_ptr key) {
    if (key != 0 && h->ah_methods->ahm_free_key) {
        h->ah_methods->ahm_free_key(key);
    }
}

static void aux_hashmap_free_value(aux_hashmap *h, aux_hashmap_ptr value) {
    if (value != 0 && h->ah_methods->ahm_free_value) {
        h->ah_methods->ahm_free_value(value);
    }
}

static bool aux_hashmap_compare_key(aux_hashmap *h, aux_hashmap_ptr key, aux_hashmap_ptr other) {
    return h->ah_methods->ahm_compare_key ? h->ah_methods->ahm_compare_key(key, other) == 0 : key == other;
}

void aux_hashmap_init(aux_hashmap *h, aux_hashmap_methods const *methods) {
    h->ah_capacity = 0;
    h->ah_load = 0;
    h->ah_buckets = 0;
    assert(methods != NULL);
    h->ah_methods = methods;
}

void aux_hashmap_release (aux_hashmap *h) {
    for (size_t i = 0; i < h->ah_capacity; i++) {
        aux_hashmap_bucket *bucket = &h->ah_buckets[i];
        if (bucket->ahb_key && h->ah_methods->ahm_free_key) {
            h->ah_methods->ahm_free_key(bucket->ahb_key);
        }
        if (bucket->ahb_value && h->ah_methods->ahm_free_value) {
            h->ah_methods->ahm_free_value(bucket->ahb_value);
        }
    }

    free(h->ah_buckets);
}

static bool aux_hashmap_insert (aux_hashmap *h, aux_hashmap_ptr key, aux_hashmap_ptr value, aux_hashmap_ptr *old_value) {
    const size_t hash = h->ah_methods->ahm_hash_key ? h->ah_methods->ahm_hash_key(key) : key;
    assert(h->ah_capacity > 0);
    assert(h->ah_buckets != NULL);

    for (size_t i = 0; i < h->ah_capacity; i++) {
        size_t idx = (hash + i) % h->ah_capacity;
        aux_hashmap_bucket *bucket = &h->ah_buckets[idx];

        if (bucket->ahb_value == 0) {
            // found an empty slot
            bucket->ahb_value = value;
            bucket->ahb_key = key;
            if (old_value) {
                *old_value = 0;
            }
            return false;
        }

        if (aux_hashmap_compare_key(h, bucket->ahb_key, key)) {
            // found the key in the map
            if (old_value) {
                *old_value = bucket->ahb_value;
            } else {
                aux_hashmap_free_value(h, bucket->ahb_value);
            }

            aux_hashmap_free_key(h, bucket->ahb_key);
            bucket->ahb_value = value;
            bucket->ahb_key = key;
            return true;
        }
    }

    assert(false);
}

bool aux_hashmap_set (aux_hashmap *h, aux_hashmap_ptr key, aux_hashmap_ptr value, aux_hashmap_ptr *old_value) {
    assert(key != 0);
    assert(value != 0);

    if (0.7 * h->ah_capacity <= h->ah_load) {
        size_t const new_capacity = h->ah_capacity == 0 ? 2 : 2 * h->ah_capacity;
        aux_hashmap_bucket *new_buckets = malloc(new_capacity * sizeof(aux_hashmap_bucket));
        assert(new_buckets != NULL);

        aux_hashmap_bucket *old_buckets = h->ah_buckets;
        size_t old_capacity = h->ah_capacity;

        h->ah_buckets = new_buckets;
        h->ah_capacity = new_capacity;
        for (size_t i = 0; i < old_capacity; i++) {
            aux_hashmap_bucket *bucket = &old_buckets[i];
            if (bucket->ahb_value != 0) {
                aux_hashmap_insert(h, bucket->ahb_key, bucket->ahb_value, NULL);
            }
        }
        free(old_buckets);
    }

    h->ah_load += 1;
    return aux_hashmap_insert(h, key, value, old_value);
}


bool aux_hashmap_get (aux_hashmap *h, aux_hashmap_ptr key, aux_hashmap_ptr *out) {
    if (key == 0 || h->ah_load == 0) {
        return false;
    }

    bool found_empty_bucket = false;
    size_t empty_bucket_idx;

    const size_t hash = h->ah_methods->ahm_hash_key ? h->ah_methods->ahm_hash_key(key) : key;
    assert(h->ah_capacity > 0);
    assert(h->ah_buckets != NULL);

    for (size_t i = 0; i < h->ah_capacity; i++) {
        size_t idx = (hash + i) % h->ah_capacity;
        aux_hashmap_bucket *bucket = &h->ah_buckets[idx];

        if (bucket->ahb_key == 0) {
            return false;
        }

        if (bucket->ahb_value == 0) {
            if (!found_empty_bucket) {
                found_empty_bucket = true;
                empty_bucket_idx = idx;
            }
            continue;
        }

        if (aux_hashmap_compare_key(h, bucket->ahb_key, key)) {
            assert(bucket->ahb_value);
            if (out) {
                *out = bucket->ahb_value;
            }

            if (found_empty_bucket) {
                aux_hashmap_bucket *empty_bucket = &h->ah_buckets[empty_bucket_idx];
                *empty_bucket = *bucket;
                bucket->ahb_value = 0;
            }

            return true;
        }
    }

    return false;
}

bool aux_hashmap_delete(aux_hashmap *h, aux_hashmap_ptr key, aux_hashmap_ptr *old_value) {
    if (key == 0 || h->ah_load == 0) {
        return false;
    }

    const size_t hash = h->ah_methods->ahm_hash_key ? h->ah_methods->ahm_hash_key(key) : key;

    for (size_t i = 0; i < h->ah_capacity; i++) {
        size_t idx = (hash + i) % h->ah_capacity;
        aux_hashmap_bucket *bucket = &h->ah_buckets[idx];

        if (bucket->ahb_key == 0) {
            return false;
        }

        if (bucket->ahb_value == 0) {
            continue;
        }

        if (aux_hashmap_compare_key(h, bucket->ahb_key, key)) {
            if (old_value != NULL) {
                *old_value = bucket->ahb_value;
            } else {
                aux_hashmap_free_value(h, bucket->ahb_value);
            }
            aux_hashmap_free_key(h, bucket->ahb_key);
            bucket->ahb_value = 0;
            h->ah_load -= 1;
            return true;
        }
    }

    return false;
}

aux_hashmap_methods const aux_hashmap_default_methods = { 0 };

#ifndef NDEBUG
#include <stdio.h>
void aux_print_hashmap(aux_hashmap const* h) {
    printf ("capacity: %zu, load: %zu\n", h->ah_capacity, h->ah_load);
    for (size_t i = 0; i < h->ah_capacity; i++) {
        aux_hashmap_bucket const* bucket = &h->ah_buckets[i];
        printf("%4zu: ", i);
        if (bucket->ahb_key == 0) {
            printf("bucket empty\n");
        } else if (bucket->ahb_value == 0) {
            printf("bucket empty (lazy deleted)\n");
        } else {
            printf("key = %lx, value = %lx\n", bucket->ahb_key, bucket->ahb_value);
        }
    }
}
#endif

