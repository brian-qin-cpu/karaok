#ifndef _RINGBUF_H__
#define _RINGBUF_H__

#include <stdio.h>

typedef struct {
	char *buffer;
	size_t size;
	volatile size_t read_pos;
	volatile size_t write_pos;
} ring_buffer_t;

int ring_init(ring_buffer_t *rb, size_t bytes);
size_t ring_avail_write(ring_buffer_t *rb);
size_t ring_avail_read(ring_buffer_t *rb);
size_t ring_write(ring_buffer_t *rb, const void *data, size_t bytes);
size_t ring_read(ring_buffer_t *rb, void *data, size_t bytes);






#endif /* _RINGBUF_H__ */
