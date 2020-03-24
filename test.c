#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "slip_encoder.h"
#include "slip_decoder.h"

void slip_encode_identity() {
  encoder enc;
  uint8_t buf[32];
  enc.buffer = buf;
  enc.size = 32;
  enc.pointer = 0;

  post_slip_encoder(&enc, "Hello, ", 7);

  assert(strncmp("Hello, ", enc.buffer, 7) == 0);
}

void slip_encode_escape_frame_end() {
  encoder enc;
  uint8_t buf[32];
  enc.buffer = buf;
  enc.size = 32;
  enc.pointer = 0;

  post_slip_encoder(&enc, "\xC0", 1);

  assert(strncmp("\xDB\xDC", enc.buffer, 2) == 0);
}

void slip_encode_escape_frame_escape() {
  encoder enc;
  uint8_t buf[32];
  enc.buffer = buf;
  enc.size = 32;
  enc.pointer = 0;

  post_slip_encoder(&enc, "\xDB", 1);

  assert(strncmp("\xDB\xDD", enc.buffer, 2) == 0);
}

void slip_encode_frame_end() {
  encoder enc;
  uint8_t buf[32];
  enc.buffer = buf;
  enc.size = 32;
  enc.pointer = 0;

  post_slip_encoder(&enc, "Hello, ", 7);
  terminate_message_slip_encoder(&enc);

  assert(strncmp("Hello, \xC0", enc.buffer, 8) == 0);
}

void slip_encode_frame_concat() {
  encoder enc;
  uint8_t buf[32];
  enc.buffer = buf;
  enc.size = 32;
  enc.pointer = 0;

  post_slip_encoder(&enc, "Hello, ", 7);
  post_slip_encoder(&enc, "world!\n", 7);
  terminate_message_slip_encoder(&enc);

  assert(strncmp("Hello, world!\n\xC0", enc.buffer, 15) == 0);
}

uint8_t decoded_buf[32];

void on_complete(uint8_t* msg, uint32_t size) {
  memcpy(decoded_buf, msg, size);
}

void slip_decode_identity() {
  decoder dec;
  uint8_t buf[32];
  dec.buffer = buf;
  dec.size = 32;
  dec.pointer = 0;

  post_slip_decoder(&dec, "Hello, ", 7, on_complete);
  assert(strncmp("Hello, ", dec.buffer, 7) == 0);
}

void slip_decode_single_fragment() {
  decoder dec;
  uint8_t buf[32];
  dec.buffer = buf;
  dec.size = 32;
  dec.pointer = 0;

  post_slip_decoder(&dec, "Hello, world!\n\xC0", 15, on_complete);
  assert(strncmp("Hello, world!\n", decoded_buf, 14) == 0);
}

int main() {
  slip_encode_identity();
  slip_encode_escape_frame_end();
  slip_encode_escape_frame_escape();
  slip_encode_frame_end();
  slip_encode_frame_concat();

  slip_decode_identity();
  slip_decode_single_fragment();

  printf("All good\n");
}
