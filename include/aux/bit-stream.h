#ifndef AUX_BIT_STREAM_H
#define AUX_BIT_STREAM_H
#include <stdint.h>
#include <stddef.h>

struct bit_stream_reader
{
	size_t bsr_offset;
	size_t bsr_size;
	uint8_t* bsr_buffer;
};

void bit_stream_reader_init (struct bit_stream_reader *r, size_t buffer_size, uint8_t buffer[buffer_size]);
void bit_stream_reader_release (struct bit_stream_reader *r) {}

void bit_stream_reader_uint_n (struct bit_stream_reader *r, uint64_t* v, size_t num_bits);


#endif // AUX_BIT_STREAM_H
