/*
 * TODO license smth
 */
#ifndef __included__ipc_h__
#define __included_ipc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct sample_ipc_mem_t;

typedef struct {
    /* Filedescriptor describing the mmap */
    int fd;

    /* describes the mmapped memory */
    struct sample_ipc_mem_t *memory;

    int size;
} sample_ipc_main_t;

extern int sample_ipc_open(sample_ipc_main_t *self);

int sample_ipc_close(sample_ipc_main_t *self);

uint32_t sample_ipc_communicate_to_server(sample_ipc_main_t *self);

void sample_ipc_communicate_to_client(sample_ipc_main_t *self);

#ifdef __cplusplus
}
#endif

#endif /* __included_ipc_h__ */
