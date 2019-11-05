/*
 * TODO license smth
 */
#ifndef __included__ringbuf_h__
#define __included_ringbuf_h__

#ifdef __cplusplus
extern "C" {
#endif

// inspired by https://stackoverflow.com/a/1771607/4108673

#include <stdint.h>

/*
12Mpps / 256 max. badge size =~ 47k badges per second
=> 47k uwords/second
=> 47k uwords/sec * 4 byte = 188kB/sec
47k uwords/sec * 10 sec = 470k uwords
188kB/sec * 10 sec = 1.88MB
*/
// has to be a power of two and smaller than 2^32
#define SAMPLE_RINGBUF_SIZE 8 // 1048576
#define SAMPLE_RINGBUF_MAP 7 // 1048575 // SAMPLE_RINGBUF_SIZE - 1
// capacity will be SAMPLE_RINGBUF_SIZE - 1

typedef struct {
	uint32_t headIndex; // next free field
	uint32_t tailIndex; // oldest field or =headIndex if ring is empty
	uint32_t sizeOfBuffer;
	uint32_t n_rx_packets[SAMPLE_RINGBUF_SIZE];
} sample_ringbuffer_t;

sample_ringbuffer_t* sample_ringbuf_init();

void sample_ringbuf_destroy(sample_ringbuffer_t *rbuf);

void sample_ringbuf_reset(sample_ringbuffer_t *rbuf);

void sample_ringbuf_push(sample_ringbuffer_t *rbuf, uint32_t data);

uint8_t sample_ringbuf_pop(sample_ringbuffer_t *rbuf, uint32_t *dst);

void sample_ringbuf_print(sample_ringbuffer_t *rbuf);

#ifdef __cplusplus
}
#endif

#endif /* __included_ringbuf_h__ */
