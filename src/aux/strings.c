#include "aux/strings.h"
#include <stdlib.h>

static void aux_string_slice_reset (aux_string_slice_t *slice)
{
    slice->string = NULL;
    slice->from   = 0;
    slice->to     = 0;
}

void aux_string_split_string (aux_string_p str, aux_string_p delim, aux_string_split_iter_t *iter)
{
    iter->string  = aux_string_copy (str);
    iter->delim   = aux_string_copy (delim);
    iter->current = 0;
}

void aux_string_split_iter_release (aux_string_split_iter_t *iter)
{
    aux_string_free (iter->string);
    aux_string_free (iter->delim);
}

bool aux_string_split_iter_next (aux_string_split_iter_t *iter, aux_string_slice_t *slice)
{
    if (iter->current < iter->string->length)
    {
        size_t next = aux_string_find_next (iter->string, iter->delim, iter->current);

        if (next == SIZE_MAX)
        {
            // This is the last piece
            slice->to = iter->string->length + 1;
        }
        else
        {
            slice->to = next;
        }

        slice->string = iter->string;
        slice->from   = iter->current;

        iter->current = slice->to + iter->delim->length;

        return true;
    }

    aux_string_slice_reset (slice);
    return false;
}

size_t aux_string_find (aux_string_p haystack, aux_string_p needle)
{
    return aux_string_find_next (haystack, needle, 0);
}

bool aux_string_slice_starts_with (aux_string_slice_t *slice, aux_string_p prefix)
{
    size_t offset    = 0;
    size_t remaining = slice->to - slice->from;

    if (remaining < prefix->length)
    {
        return false;
    }

    while (offset < prefix->length)
    {
        if (prefix->buffer[offset] != slice->string->buffer[slice->from + offset])
        {
            return false;
        }

        offset += 1;
    }

    return true;
}

aux_string_slice_t aux_string_slice_all (aux_string_p string)
{
    return (aux_string_slice_t){ .string = string, .from = 0, .to = string->length };
}

aux_string_slice_t aux_string_slice_from (aux_string_p string, size_t offset)
{
    return (aux_string_slice_t){ .string = string, .from = offset, .to = string->length };
}

aux_string_slice_t aux_string_slice_to (aux_string_p string, size_t to)
{
    return (aux_string_slice_t){ .string = string, .from = 0, .to = to };
}

aux_string_slice_t aux_string_slice_from_to (aux_string_p string, size_t from, size_t to)
{
    return (aux_string_slice_t){ .string = string, .from = from, .to = to };
}

size_t aux_string_find_next (aux_string_p haystack, aux_string_p needle, size_t offset)
{
    while (offset < haystack->length)
    {
        size_t remaining = haystack->length - offset;

        if (remaining < needle->length)
        {
            return SIZE_MAX;
        }

        aux_string_slice_t slice = aux_string_slice_from (haystack, offset);

        if (aux_string_slice_starts_with (&slice, needle))
        {
            // Woho found something
            return offset;
        }

        offset += 1;
    }

    return SIZE_MAX;
}

bool aux_string_starts_with (aux_string_p str, aux_string_p prefix)
{
    aux_string_slice_t slice = aux_string_slice_all (str);
    return aux_string_slice_starts_with (&slice, prefix);
}

uint8_t aux_string_at (const aux_string_p str, size_t idx)
{
    if (str && str->length > idx)
    {
        return str->buffer[idx];
    }

    return 0;
}

const uint8_t *aux_string_get_buffer (const aux_string_p str)
{
    return &str->buffer[0];
}

const char *aux_string_as_c_string (const aux_string_p str)
{
    return (char *) aux_string_get_buffer (str);
}

size_t aux_string_length (aux_string_p str)
{
    return str->length;
}

aux_string_p aux_string_alloc (size_t length)
{
    aux_string_p str = malloc (length + 1 + sizeof (aux_string_t));
    if (str)
    {
        str->length         = length;
        str->ref            = 1;
        str->buffer[length] = 0;
        return str;
    }

    return NULL;
}

aux_string_p aux_string_from_slice (aux_string_slice_t slice)
{
    return aux_string_from_buffer (&slice.string->buffer[slice.from], slice.to - slice.from);
}

aux_string_p aux_string_from_buffer (const void *b, size_t size)
{
    aux_string_p str = aux_string_alloc (size);
    if (str)
    {
        memcpy (&str->buffer[0], b, size);
        return str;
    }

    return NULL;
}

aux_string_p aux_string_from_c_string (const char *b)
{
    size_t length = strlen (b);
    return aux_string_from_buffer (b, length);  // copy zero byte
}

void aux_string_free (aux_string_p str)
{
    if (str)
    {
        if (--str->ref == 0)
        {
            // refcount is now zero
            free (str);
        }
    }
}

aux_string_p aux_string_copy (aux_string_p str)
{
    if (str)
    {
        str->ref += 1;
        return str;
    }

    return NULL;
}

aux_string_p aux_string_dup (aux_string_p str)
{
    if (str)
    {
        return aux_string_from_buffer (aux_string_get_buffer (str), aux_string_length (str));
    }

    return NULL;
}
/*
aux_string_t *_aux_string_concat (aux_string_p a, aux_string_p b)
{
    if (a->length == 0)
    {
        return aux_string_copy (b);
    }
    else if (b->length == 0)
    {
        return aux_string_copy (a);
    }
    else
    {
        aux_string_p str = aux_string_alloc (a->length + b->length);
        if (str)
        {
            memcpy (&str->buffer[0], aux_string_get_buffer (a), a->length);
            memcpy (&str->buffer[a->length], aux_string_get_buffer (b), b->length);
            return str;
        }
    }

    return NULL;
}*/

aux_string_p _aux_string_concat_multi (size_t n, ...)
{
    size_t total_length = 0;

    {
        // count the lengths
        va_list args;
        va_start (args, n);
        for (size_t i = 0; i < n; i++)
        {
            total_length += aux_string_length (va_arg (args, aux_string_p));
        }
        va_end (args);
    }

    aux_string_p str = aux_string_alloc (total_length);
    if (str)
    {
        size_t   offset = 0;
        uint8_t *buffer = str->buffer;
        // now copy all contents
        va_list args;
        va_start (args, n);
        for (size_t i = 0; i < n; i++)
        {
            aux_string_p other = va_arg (args, aux_string_p);
            memcpy (buffer + offset, aux_string_get_buffer (other), aux_string_length (other));
            offset += aux_string_length (other);
        }
        va_end (args);
    }

    return str;
}

uint64_t aux_string_hash (aux_string_p a)
{
    return aux_string_slice_hash (aux_string_slice_all (a));
}

uint64_t aux_string_slice_hash (aux_string_slice_t a)
{
    uint64_t hash = 0;
    for (size_t i = a.from; i < a.to; i++)
    {
        hash = 33 * hash + a.string->buffer[i];
    }
    return hash;
}

bool aux_string_equals (aux_string_p a, aux_string_p b)
{
    if (a == b)
    {
        return true;
    }

    if (a == NULL || b == NULL)
    {
        return false;
    }

    size_t length = aux_string_length (a);
    if (length != aux_string_length (b))
    {
        return false;
    }

    return memcmp (aux_string_get_buffer (a), aux_string_get_buffer (b), length) == 0;
}

int aux_string_cmp (aux_string_p a, aux_string_p b)
{
    const uint8_t *a_ptr = aux_string_get_buffer (a);
    const uint8_t *b_ptr = aux_string_get_buffer (b);

    while (*a_ptr && *a_ptr == *b_ptr)
    {
        a_ptr++, b_ptr++;
    }

    return *a_ptr - *b_ptr;
}

size_t aux_string_slice_length (aux_string_slice_t slice)
{
    return slice.to - slice.from;
}

uint8_t aux_string_slice_at (aux_string_slice_t slice, size_t at)
{
    if (at < aux_string_slice_length (slice))
    {
        return aux_string_at (slice.string, slice.from + at);
    }

    return 0;
}

bool aux_string_slice_equals_c_string (aux_string_slice_t a, const char *b)
{
    for (size_t i = 0; *b; ++i, ++b)
    {
        if (*b != (char) aux_string_slice_at (a, i))
        {
            return false;
        }
    }

    return true;
}

bool aux_string_equals_c_string (aux_string_p a, const char *b)
{
    for (size_t i = 0; i < aux_string_length (a); ++i, ++b)
    {
        if ((char) aux_string_at (a, i) != *b)
        {
            return false;
        }
    }

    return *b == '\0';
}
