#include "aux/string-builder.h"
#include "assert.h"
#include <stdlib.h>

bool aux_string_builder_init_capacity (aux_string_builder_t *builder, size_t capacity)
{
    builder->length = 0;
    builder->result = realloc (NULL, sizeof (aux_string_t) + capacity);
    if (builder->result)
    {
        builder->result->length = capacity;
        return true;
    }

    return false;
}

uint8_t *aux_string_builder_grow (aux_string_builder_t *builder, size_t delta)
{
    aux_string_p result       = builder->result;
    size_t       requiredSize = builder->length + delta;

    if (result == NULL)
    {
        return NULL;
    }

    if (result->length <= requiredSize)
    {
        size_t new_size = result->length;
        while (new_size < requiredSize)
        {
            new_size <<= 1;
        }

        aux_string_p new_result = realloc (builder->result, sizeof (aux_string_t) + new_size);
        if (new_result == NULL)
        {
            return NULL;
        }

        new_result->length = new_size;
        builder->result = result = new_result;
    }

    size_t offset = builder->length;
    builder->length += delta;
    return &result->buffer[offset];
}

void aux_string_builder_init (aux_string_builder_t *builder)
{
    aux_string_builder_init_capacity (builder, FOO_STRING_BUILDER_DEFAULT_CAPACITY);
}

void aux_string_builder_free (aux_string_builder_t *builder)
{
    if (builder->result)
    {
        aux_string_free (builder->result);
    }
}

void aux_string_builder_append_c (aux_string_builder_t *builder, char c)
{
    uint8_t *ptr = aux_string_builder_grow (builder, 1);
    if (ptr)
    {
        *ptr = c;
    }
}

void aux_string_builder_append_c_times (aux_string_builder_t *builder, char c, size_t times)
{
    uint8_t *ptr = aux_string_builder_grow (builder, times);
    if (ptr)
    {
        memset (ptr, c, times);
    }
}

void aux_string_builder_append_buffer (aux_string_builder_t *builder, const uint8_t *c, size_t length)
{
    uint8_t *ptr = aux_string_builder_grow (builder, length);
    if (ptr)
    {
        memcpy (ptr, c, length);
    }
}

void aux_string_builder_append_c_nstring (aux_string_builder_t *builder, const char *c, size_t max)
{
    size_t length = strlen (c);
    aux_string_builder_append_buffer (builder, (const uint8_t *) c, max < length ? max : length);
}

void aux_string_builder_append_c_string (aux_string_builder_t *builder, const char *c)
{
    aux_string_builder_append_buffer (builder, (const uint8_t *) c, strlen (c));
}

void aux_string_builder_append_string (aux_string_builder_t *builder, aux_string_p str)
{
    aux_string_builder_append_buffer (builder, aux_string_get_buffer (str), aux_string_length (str));
}

void aux_string_builder_append_nstring (aux_string_builder_t *builder, aux_string_p str, size_t max)
{
    size_t length = aux_string_length (str);
    aux_string_builder_append_buffer (builder, aux_string_get_buffer (str), max < length ? max : length);
}

void aux_string_builder_append_uint (aux_string_builder_t *builder, uint64_t v)
{
    size_t  i = 0;
    uint8_t buffer[32];

    do
    {
        buffer[32 - ++i] = '0' + (v % 10);
        v /= 10;
    } while (v > 0);

    aux_string_builder_append_buffer (builder, buffer + (32 - i), i);
}

void aux_string_builder_append_int (aux_string_builder_t *builder, int64_t v)
{
    size_t  i = 0;
    uint8_t buffer[32];
    bool    neg = false;

    if (v < 0)
    {
        neg = true;
        v   = -v;
    }

    do
    {
        buffer[32 - ++i] = '0' + (v % 10);
        v /= 10;
    } while (v > 0);

    if (neg)
    {
        aux_string_builder_append_c (builder, '-');
    }

    aux_string_builder_append_buffer (builder, buffer + (32 - i), i);
}

void aux_string_builder_append_float (aux_string_builder_t *builder, double f)
{
    (void) builder;
    (void) f;
    abort(); // not implemented
}

aux_string_p aux_string_builder_finish (aux_string_builder_t *builder)
{
    aux_string_p result = builder->result;
    if (result)
    {
        result->buffer[builder->length] = 0;
        result->length                  = builder->length;
        result->ref                     = 1;
    }

    *builder = FOO_STRING_BUILDER_EMPTY;
    return result;
}
