#ifndef TIME_HELPER_H
#define TIME_HELPER_H
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TIME_NANOSECOND ((uint64_t) 1)
#define TIME_MICROSECOND (1000 * TIME_NANOSECOND)
#define TIME_MILLISECOND (1000 * TIME_MICROSECOND)
#define TIME_SECOND (1000 * TIME_MILLISECOND)

#define TIME_FLOOR_MULTIPLES(ts, m) (((ts) + (m) - 1) / (m))
#define TIME_FLOOR_US(ts) TIME_FLOOR_MULTIPLES(ts, TIME_MICROSECOND)
#define TIME_FLOOR_MS(ts) TIME_FLOOR_MULTIPLES(ts, TIME_MILLISECOND)

static uint64_t clock_get_time_ns_uint64 ()
{
    struct timespec tp;

    int result = clock_gettime (CLOCK_MONOTONIC, &tp);
    if (result == 0)
    {
        return TIME_SECOND * (uint64_t) tp.tv_sec + TIME_NANOSECOND * (uint64_t) tp.tv_nsec;
    }
    fprintf (stderr, "Error on clock_gettime: %s (%d)\n", strerror (errno), errno);
    abort ();
}

static inline void sleep_until_ns (uint64_t until_ns)
{
    while (true)
    {
        uint64_t now = clock_get_time_ns_uint64 ();

        if (now > until_ns)
        {
            break;
        }

        uint64_t delta = until_ns - now;

        struct timespec ts = { .tv_sec = delta / TIME_SECOND, .tv_nsec = delta % TIME_SECOND };
        nanosleep (&ts, NULL);
    }
}

#endif
