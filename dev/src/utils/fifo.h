#ifndef __FIFO_H
#define __FIFO_H

#include <stdint.h>
#include <stdbool.h>

struct fifo_buffer;

/* Fifo capacity must be a power of 2
 * Thread-safe implementation without using of any mutex
 * Based on principle that write pointer is always bigger or equal to read poiner
*/
void fifo_init(struct fifo_buffer *rb, uint8_t *buffer, uint32_t capacity);
/* Write to end of fifo. Return num bytes written */
uint32_t fifo_write(struct fifo_buffer *rb, uint8_t *data, uint32_t len);
/* Read/remove from start of fifo. Return num bytes read */
uint32_t fifo_read(struct fifo_buffer *rb, uint8_t *data, uint32_t len);
/* Flush the whole buffer to a specifed function */
uint32_t fifo_flush_to(struct fifo_buffer *rb, void (*flush_func)(uint8_t *buffer, uint32_t length));
/* Return space left in the fifo */
uint32_t fifo_free_bytes(struct fifo_buffer *rb);
/* Return space used in the fifo */
uint32_t fifo_used_bytes(struct fifo_buffer *rb);
/* Empty the fifo */
void fifo_set_empty(struct fifo_buffer *rb);

struct fifo_buffer {
    uint8_t *buffer;
    uint32_t read_index;
    uint32_t write_index;
    uint32_t capacity;
};

#endif
