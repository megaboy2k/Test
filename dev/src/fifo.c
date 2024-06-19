#include <string.h>
#include <stddef.h>
#include "fifo.h"


void fifo_init( struct fifo_buffer *rb, uint8_t *buffer, uint32_t capacity )
{
    rb->buffer = buffer;
    rb->read_index = 0;
    rb->write_index = 0;
    rb->capacity = capacity;
}

uint32_t fifo_write( struct fifo_buffer *rb, uint8_t *data, uint32_t len )
{
    uint32_t rc = 0;
    uint32_t read_index = rb->read_index; /* read pointer must be kept unchanged across the function */
    uint32_t delta = rb->write_index - read_index;

    if((delta + len) > rb->capacity) len = rb->capacity - delta;

    if(!len) return rc; // nothing to store

    if(((rb->write_index % rb->capacity) + len) > rb->capacity)
    {
        uint32_t size_hi = rb->capacity - (rb->write_index % rb->capacity);

        memcpy(&rb->buffer[rb->write_index % rb->capacity], data, size_hi);
        memcpy(rb->buffer, &data[size_hi], len - size_hi);
    }
    else
    {
        memcpy(&rb->buffer[rb->write_index % rb->capacity], data, len);
    }
    rb->write_index += len;
    rc = len;
    return rc;
}

uint32_t fifo_read( struct fifo_buffer *rb, uint8_t *data, uint32_t len )
{
    uint32_t    rc = 0;
    uint32_t    write_index = rb->write_index; /* write pointer must be kept unchanged across the function */
    uint32_t    delta = write_index - rb->read_index;

    if(len > delta) len = delta;
    if(!len) return rc; // nothing to read

    if(((rb->read_index % rb->capacity) + len) > rb->capacity)
    {
        uint32_t size_hi = rb->capacity - (rb->read_index % rb->capacity);

        memcpy(data, &rb->buffer[rb->read_index % rb->capacity], size_hi);
        memcpy(&data[size_hi], rb->buffer, len - size_hi);
    }
    else
    {
        memcpy(data, &rb->buffer[rb->read_index % rb->capacity], len);
    }
    rb->read_index += len;
    rc = len;
    return rc;
}

uint32_t fifo_flush_to( struct fifo_buffer *rb, void (*flush_func)(uint8_t *buffer, uint32_t length ))
{
    uint32_t len = rb->write_index - rb->read_index;
    if(!len) return len; /* nothing to read */

    if(((rb->read_index % rb->capacity) + len) > rb->capacity)
    {
        uint32_t size_hi = rb->capacity - (rb->read_index % rb->capacity);

        flush_func(&rb->buffer[rb->read_index % rb->capacity], size_hi);
        flush_func(rb->buffer, len - size_hi);
    }
    else
    {
        flush_func(&rb->buffer[rb->read_index % rb->capacity], len);
    }
    rb->read_index += len;
    return len;
}

uint32_t fifo_free_bytes( struct fifo_buffer *rb )
{
    return rb->capacity - (rb->write_index - rb->read_index);
}

uint32_t fifo_used_bytes( struct fifo_buffer *rb )
{
    return rb->write_index - rb->read_index;
}

void fifo_set_empty( struct fifo_buffer *rb )
{
    rb->read_index = rb->write_index;
}

