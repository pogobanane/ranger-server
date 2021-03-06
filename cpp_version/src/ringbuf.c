/*
 * TODO license smth
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <ringbuf.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// TODO use rbuf->sizeOfBuffer after creation during runtime

void sample_ringbuf_init(sample_ringbuffer_t* rbuf)
{
	memset(rbuf, 0, sizeof(sample_ringbuffer_t));
	rbuf->sizeOfBuffer = SAMPLE_RINGBUF_SIZE;
	sample_ringbuf_reset(rbuf);
}

void sample_ringbuf_reset(sample_ringbuffer_t *rbuf)
{
	rbuf->headIndex = 0;
	rbuf->tailIndex = 0;
}

// push onto top
void sample_ringbuf_push(sample_ringbuffer_t *rbuf, uint16_t port_id, uint16_t queue_id, uint32_t n_rx_packets)
{
	rbuf->data[rbuf->headIndex].port_id = port_id;
	rbuf->data[rbuf->headIndex].queue_id = queue_id;
	rbuf->data[rbuf->headIndex].n_rx_packets = n_rx_packets;

	// update headIndex
	// this acts as (idx+1)%size because size is a power of two
	rbuf->headIndex += 1;
	rbuf->headIndex &= SAMPLE_RINGBUF_MAP;
	if (rbuf->headIndex == rbuf->tailIndex )
	{
		// if we overwrite a 
		rbuf->tailIndex += 1;
		rbuf->tailIndex &= SAMPLE_RINGBUF_MAP;
	}
}

// remove from bottom and memcpy data into dst
// return number of returned sample_ringbuffer_data_ts (0 or 1)
uint8_t sample_ringbuf_pop(sample_ringbuffer_t *rbuf, sample_ringbuffer_data_t *dst)
{
	if (rbuf->headIndex == rbuf->tailIndex) // rbuf is empty
		return 0;
	memcpy(dst, &(rbuf->data[rbuf->tailIndex]), sizeof(sample_ringbuffer_data_t));
	rbuf->tailIndex += 1;
	rbuf->tailIndex &= SAMPLE_RINGBUF_MAP;

	return 1;
}

// number of items in the ringbuffer
uint32_t sample_ringbuf_count(sample_ringbuffer_t *rbuf) {
	if (rbuf->headIndex == rbuf->tailIndex)
		return 0;
	if (rbuf->headIndex > rbuf->tailIndex)
		return rbuf->headIndex - rbuf->tailIndex;
	if (rbuf->headIndex < rbuf->tailIndex)
		return rbuf->headIndex - 1 + (SAMPLE_RINGBUF_SIZE - rbuf->tailIndex + 1);
}

#include <stdio.h>
#include <inttypes.h>
void sample_ringbuf_print(sample_ringbuffer_t *rbuf)
{
	printf("Ringbuffer: \n");
	printf("#,port_id,queue_id,n_rx_packets\n");
	for (uint32_t i = 0; i<SAMPLE_RINGBUF_SIZE; i++) {
		sample_ringbuffer_data_t data = rbuf->data[i];
		printf("%" PRIu32 ": %" PRIu16 ",%" PRIu16 ",%" PRIu32 "\n", i, data.port_id, data.queue_id, data.n_rx_packets);
	}
	printf("headIndex: %" PRIu32 "\n", rbuf->headIndex);
	printf("tailIndex: %" PRIu32 "\n", rbuf->tailIndex);
}

// requirement: sizeof(*data) == rbuf->sizeOfBuffer
// *data is the destination
// returns number of uwords copied
uint32_t sample_ringbuf_extract_all(sample_ringbuffer_t *rbuf, sample_ringbuffer_data_t *data)
{
	// TODO not implemented because: takes too much brainpower to implement with hardly any performance gain
	return 0;

	//uint32_t start_idx = rbuf->tailIndex;
	if (rbuf->tailIndex <= rbuf->headIndex)
	{
		uint32_t n_copy = rbuf->headIndex - rbuf->tailIndex;
		memcpy(data, rbuf->data , sizeof(sample_ringbuffer_data_t) * n_copy);
	} else // tailIndex > headIndex
	{
		uint32_t n_copy = rbuf->headIndex - rbuf->tailIndex;
		memcpy(data, rbuf->data, sizeof(sample_ringbuffer_data_t) * n_copy);
		//memcpy()
	}
	//uint32_t stop_idx = min(start_idx, )
}

#ifdef __cplusplus
}
#endif