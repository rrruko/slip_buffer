/*
 * We want to SLIP-encode messages to send them to a device.
 * This involves posting the messages to the encoder and telling it where
 * the message boundaries should be. So we want
 *
 * -- Put a message fragment into the encoder
 * void post_slip_encoder(encoder* enc, uint8_t* msg, uint32_t size);
 *
 * -- Terminate the current message
 * void terminate_message_slip_encoder(encoder* enc);
 *
 * -- Read a message from the encoder into the buffer
 * int read_slip_encoder(encoder* enc, uint8_t* buffer, uint32_t size);
 *
 * That way, we could do something like
 *
 * post_slip_encoder(&enc, "Hello, ", 7);
 * post_slip_encoder(&enc, "world!\n", 7);
 * terminate_message_slip_encoder(&enc);
 *
 *
 * Which would produce the same state in the encoder as
 *
 * post_slip_encoder(&enc, "Hello, world!\n", 14);
 * terminate_message_slip_encoder(&enc);
 *
 * And result in the same exact bytes getting written to the buffer when
 * we call read_slip_encoder; namely, the string "Hello, world!\n" followed by
 * a byte 0xC0.
 */

#include <stdint.h>

#include "slip_encoder.h"

void post_slip_encoder(encoder* enc, uint8_t* msg, uint32_t size) {
  int j = enc->pointer;
  for (int i = 0; i < size; i++) {
    if (msg[i] == END) {
      enc->buffer[j] = ESC;
      j++;
      enc->buffer[j] = ESC_END;
    } else if (msg[i] == ESC) {
      enc->buffer[j] = ESC;
      j++;
      enc->buffer[j] = ESC_ESC;
    } else {
      enc->buffer[j] = msg[i];
    }
    j++;
  }
  enc->pointer = j;
}

void terminate_message_slip_encoder(encoder* enc) {
  enc->buffer[enc->pointer] = END;
}
