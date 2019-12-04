#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "ringbuf.h"

int ringbuf_test() {
  if (8 != SAMPLE_RINGBUF_SIZE) {
    printf("SAMPLE_RINGBUF_SIZE and _MAP have to be set to 8 and 7 for this test");
  }

  sample_ringbuffer_t *rbuf = (sample_ringbuffer_t*) malloc(sizeof(sample_ringbuffer_t));
  sample_ringbuf_init(rbuf);
  sample_ringbuffer_data_t dst;
  uint8_t retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 0)
    return 1;

  sample_ringbuf_print(rbuf);
  printf("push 1337\n");
  sample_ringbuf_push(rbuf, 1, 2, 1337);
  sample_ringbuf_print(rbuf);
  printf("pop\n");
  retcode = sample_ringbuf_pop(rbuf, &dst);
  sample_ringbuf_print(rbuf);
  if (retcode != 1 || dst.n_rx_packets != 1337)
    return 2;
  printf("pop\n");
  retcode = sample_ringbuf_pop(rbuf, &dst);
  sample_ringbuf_print(rbuf);
  if (retcode != 0)
    return 3;

  printf("push a lot\n");
  sample_ringbuf_push(rbuf, 1, 2, 1338);
  sample_ringbuf_push(rbuf, 1, 2, 1339);
  sample_ringbuf_push(rbuf, 1, 2, 1340);
  sample_ringbuf_push(rbuf, 1, 2, 1341);
  sample_ringbuf_push(rbuf, 1, 2, 1342);
  sample_ringbuf_push(rbuf, 1, 2, 1343);
  sample_ringbuf_push(rbuf, 1, 2, 1344);
  sample_ringbuf_push(rbuf, 1, 2, 1345);
  sample_ringbuf_push(rbuf, 1, 2, 1346);
  sample_ringbuf_print(rbuf);
  printf("pop a lot\n");
  retcode = sample_ringbuf_pop(rbuf, &dst);
  printf("got: %" PRIu32 "\n", dst.n_rx_packets);
  if (retcode != 1 || dst.n_rx_packets != 1340)
    return 6;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst.n_rx_packets != 1341)
    return 7;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst.n_rx_packets != 1342)
    return 8;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst.n_rx_packets != 1343)
    return 9;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst.n_rx_packets != 1344)
    return 10;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst.n_rx_packets != 1345)
    return 11;
  retcode = sample_ringbuf_pop(rbuf, &dst);
  if (retcode != 1 || dst.n_rx_packets != 1346)
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