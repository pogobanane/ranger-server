/*
 * TODO license smth
 */
#ifndef __included__ipc_h__
#define __included_ipc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "ringbuf.h"

struct sample_ipc_mem_t;

/* 
 * message to client
 * client: read only
 * server: write
 */
typedef struct {
    uint32_t poll1;
    uint32_t udelay;
    uint32_t poll2;
    uint32_t usleep;
    uint32_t poll3;
    uint32_t use_interrupt; // actually a boolean
    uint32_t poll4;
} sample_ipc_for_client_t;

typedef struct {
    /* Filedescriptor describing the mmap */
    int fd;

    /* describes the mmapped memory */
    struct sample_ipc_mem_t *memory;

    /* read only message for client which is being
     * updated while communicating to server */
    sample_ipc_for_client_t last_response;

    int size;
} sample_ipc_main_t;

/* message sent from client to server 
 * messages from server to client are unint32_t
*/
typedef struct {
	uint32_t n_rx_packets;
} sample_ipc_for_server_t;

extern int sample_ipc_open(sample_ipc_main_t *self);

int sample_ipc_close(sample_ipc_main_t *self);

void sample_ipc_communicate_to_server(sample_ipc_main_t *self, uint16_t port_id, uint16_t queue_id, uint32_t n_rx_packets);

void sample_ipc_communicate_to_client(sample_ipc_main_t *self, sample_ipc_for_client_t *response, sample_ringbuffer_t* request);

#ifdef __cplusplus
}
#endif

#endif /* __included_ipc_h__ */
