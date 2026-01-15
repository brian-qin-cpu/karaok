#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ringbuf.h"

int ring_init(ring_buffer_t *rb, size_t bytes)
{
	rb->buffer = (char *)malloc(bytes);
	if (!rb->buffer)
		return -1;
	rb->size = bytes;
	rb->read_pos = 0;
	rb->write_pos = 0;
	return 0;
}

size_t ring_avail_write(ring_buffer_t *rb)
{
	return rb->size - (rb->write_pos - rb->read_pos);
}

size_t ring_avail_read(ring_buffer_t *rb)
{
	return rb->write_pos - rb->read_pos;
}

size_t ring_write(ring_buffer_t *rb, const void *data, size_t bytes)
{
	size_t avail = rb->size - (rb->write_pos - rb->read_pos);
	if (bytes > avail)
		bytes = avail;
	size_t tail_left = rb->size - rb->write_pos % rb->size;
	size_t first_part = (bytes <= tail_left) ? bytes : tail_left;
	memcpy(rb->buffer + (rb->write_pos % rb->size), data, first_part);
	if (bytes > tail_left) {
		memcpy(rb->buffer, (const char*)data + first_part, bytes - first_part);
	}
	__sync_fetch_and_add(&rb->write_pos, bytes);

	return bytes;
}

size_t ring_read(ring_buffer_t *rb, void *data, size_t bytes)
{
	size_t avail_to_read = rb->write_pos - rb->read_pos;

	if (bytes > avail_to_read)
		bytes = avail_to_read;
	size_t tail_left = rb->size - rb->read_pos % rb->size;
	size_t first_part = (bytes <= tail_left) ? bytes : tail_left;
	memcpy(data, rb->buffer + rb->read_pos % rb->size, first_part);
	if (bytes > tail_left) {
		memcpy(data + first_part, rb->buffer, bytes - first_part);
	}
	__sync_fetch_and_add(&rb->read_pos, bytes);

	return bytes;
}

