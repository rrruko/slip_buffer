#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "slip_encoder.h"
#include "slip_decoder.h"

void slip_encode_identity() {
  encoder enc;
  uint8_t buf[32];
  init_slip_encoder(&enc, buf, sizeof(buf));

  post_slip_encoder(&enc, "Hello, ", 7);

  assert(strncmp("Hello, ", enc.buffer, 7) == 0);
}

void slip_encode_escape_frame_end() {
  encoder enc;
  uint8_t buf[32];
  init_slip_encoder(&enc, buf, sizeof(buf));

  post_slip_encoder(&enc, "\xC0", 1);

  assert(strncmp("\xDB\xDC", enc.buffer, 2) == 0);
}

void slip_encode_escape_frame_escape() {
  encoder enc;
  uint8_t buf[32];
  init_slip_encoder(&enc, buf, sizeof(buf));

  post_slip_encoder(&enc, "\xDB", 1);

  assert(strncmp("\xDB\xDD", enc.buffer, 2) == 0);
}

void slip_encode_frame_end() {
  encoder enc;
  uint8_t buf[32];
  init_slip_encoder(&enc, buf, sizeof(buf));

  post_slip_encoder(&enc, "Hello, ", 7);
  terminate_message_slip_encoder(&enc);

  assert(strncmp("Hello, \xC0", enc.buffer, 8) == 0);
}

void slip_encode_frame_concat() {
  encoder enc;
  uint8_t buf[32];
  init_slip_encoder(&enc, buf, sizeof(buf));

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
  init_slip_decoder(&dec, buf, sizeof(buf));

  post_slip_decoder(&dec, "Hello, ", 7, on_complete);
  assert(strncmp("Hello, ", dec.buffer, 7) == 0);
}

void slip_decode_single_fragment() {
  decoder dec;
  uint8_t buf[32];
  init_slip_decoder(&dec, buf, sizeof(buf));

  post_slip_decoder(&dec, "Hello, world!\n\xC0", 15, on_complete);
  assert(strncmp("Hello, world!\n", decoded_buf, 14) == 0);
}

void slip_decode_multi_fragment() {
  decoder dec;
  uint8_t buf[32];
  init_slip_decoder(&dec, buf, sizeof(buf));

  post_slip_decoder(&dec, "Hello, ", 7, on_complete);
  post_slip_decoder(&dec, "world!\n\xC0", 8, on_complete);
  assert(strncmp("Hello, world!\n", decoded_buf, 14) == 0);
}

void slip_decode_multi_message() {
  decoder dec;
  uint8_t buf[32];
  init_slip_decoder(&dec, buf, sizeof(buf));

  post_slip_decoder(&dec, "Hello, ", 7, on_complete);
  post_slip_decoder(&dec, "world!\n\xC0", 8, on_complete);
  assert(strncmp("Hello, world!\n", decoded_buf, 14) == 0);

  post_slip_decoder(&dec, "cha-la ", 7, on_complete);
  post_slip_decoder(&dec, "head-", 5, on_complete);
  post_slip_decoder(&dec, "cha-la\xC0", 7, on_complete);
  assert(strncmp("cha-la head-cha-la", decoded_buf, 18) == 0);
}

void slip_decode_special_chars() {
  decoder dec;
  uint8_t buf[64];
  init_slip_decoder(&dec, buf, sizeof(buf));

  post_slip_decoder(&dec, "THERE ", 6, on_complete);
  post_slip_decoder(&dec, "IS \xDB\xDC", 5, on_complete);
  post_slip_decoder(&dec, "NO \xDB\xDD", 5, on_complete);
  post_slip_decoder(&dec, "PLANET \xDB\xDD\xDD", 10, on_complete);
  post_slip_decoder(&dec, "B\n\xC0", 3, on_complete);
  assert(strncmp("THERE IS \xC0NO \xDBPLANET \xDB\xDD" "B\n", decoded_buf, 25) == 0);
}

uint8_t round_trip_decoded_buf[512];
uint32_t ptr = 0;
void round_trip_on_complete(uint8_t* buf, uint32_t size) {
  memcpy(round_trip_decoded_buf + ptr, buf, size);
  ptr += size;
}

void slip_round_trip() {
  encoder enc;
  uint8_t enc_buf[512];
  init_slip_encoder(&enc, enc_buf, sizeof(enc_buf));

  decoder dec;
  uint8_t dec_buf[512];
  init_slip_decoder(&dec, dec_buf, sizeof(dec_buf));

  memset(round_trip_decoded_buf, 0, sizeof(round_trip_decoded_buf));

  uint8_t init_bytes[256];
  for (uint16_t i = 0; i < 256; i++) {
    init_bytes[i] = (i & 0xff) + 0x01;
  }

  for (uint16_t i = 0; i < 256; i++) {
    uint8_t buf[1];
    buf[0] = init_bytes[i];
    if (i % 16 == 0 && i > 0) {
      terminate_message_slip_encoder(&enc);
    }
    post_slip_encoder(&enc, buf, 1);
  }

  uint8_t esc_end[3] = { ESC, ESC_END, 0x00 };
  uint8_t esc_esc[3] = { ESC, ESC_ESC, 0x00 };
  assert(strstr(enc.buffer, (char*)esc_end));
  assert(strstr(enc.buffer, (char*)esc_esc));

  post_slip_decoder(&dec, enc.buffer, 512, round_trip_on_complete);
  assert(strncmp(round_trip_decoded_buf, init_bytes, 256) == 0);
}

int main() {
  slip_encode_identity();
  slip_encode_escape_frame_end();
  slip_encode_escape_frame_escape();
  slip_encode_frame_end();
  slip_encode_frame_concat();

  slip_decode_identity();
  slip_decode_single_fragment();
  slip_decode_multi_fragment();
  slip_decode_multi_message();
  slip_decode_special_chars();

  slip_round_trip();

  printf("All good\n");
}
