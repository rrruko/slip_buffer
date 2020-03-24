#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "slip_encoder.h"

void slip_encode_identity() {
  encoder enc;
  uint8_t buf[32];
  enc.buffer = buf;
  enc.size = 32;

  post_slip_encoder(&enc, "Hello, ", 7);

  assert(strncmp("Hello, ", enc.buffer, 7) == 0);
}

int main() {
  slip_encode_identity();
  printf("All good\n");
}
