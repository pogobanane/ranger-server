/*
 * TODO license smth
 */
#ifndef __included__ipc_h__
#define __included_ipc_h__

#include <stdint.h>

typedef struct {
    /* Filedescriptor describing the mmap */
    int fd;

    /* Memory pointer to mmap */
    void *memory;

    int size;
} sample_ipc_main_t;

extern int sample_ipc_open(sample_ipc_main_t *self);

int sample_ipc_close(sample_ipc_main_t *self);

#endif /* __included_ipc_h__ */
