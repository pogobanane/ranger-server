#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "ringbuf.h"

int ringbuf_test() {
  sample_ringbuffer_t *rbuf = (sample_ringbuffer_t*) malloc(sizeof(sample_ringbuffer_t));
  sample_ringbuf_init(rbuf);
  uint32_t dst = 0;
  uint8_t retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 0)
    return 1;

  sample_ringbuf_print(rbuf);
  printf("push 1337\n");
  sample_ringbuf_push(rbuf, 1337);
  sample_ringbuf_print(rbuf);
  printf("pop\n");
  retcode = sample_ringbuf_pop(rbuf, &dst);
  sample_ringbuf_print(rbuf);
  if (retcode != 1 || dst != 1337)
    return 2;
  printf("pop\n");
  retcode = sample_ringbuf_pop(rbuf, &dst);
  sample_ringbuf_print(rbuf);
  if (retcode != 0)
    return 3;

  printf("push a lot\n");
  sample_ringbuf_push(rbuf, 1338);
  sample_ringbuf_push(rbuf, 1339);
  sample_ringbuf_push(rbuf, 1340);
  sample_ringbuf_push(rbuf, 1341);
  sample_ringbuf_push(rbuf, 1342);
  sample_ringbuf_push(rbuf, 1343);
  sample_ringbuf_push(rbuf, 1344);
  sample_ringbuf_push(rbuf, 1345);
  sample_ringbuf_push(rbuf, 1346);
  sample_ringbuf_print(rbuf);
  printf("pop a lot\n");
  retcode = sample_ringbuf_pop(rbuf, &dst);
  printf("got: %" PRIu32 "\n", dst);
  if (retcode != 1 || dst != 1340)
    return 6;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst != 1341)
    return 7;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst != 1342)
    return 8;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst != 1343)
    return 9;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst != 1344)
    return 10;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst != 1345)
    return 11;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst != 1346)
    return 12;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 0)
    return 12;
  sample_ringbuf_print(rbuf);

  return 0;
}

#ifdef __cplusplus
}
#endif