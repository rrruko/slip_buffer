#include "slip_decoder.h"


void init_slip_decoder(decoder* dec, uint8_t* buf, uint32_t size) {
  dec->buffer = buf;
  dec->size = size;
  dec->pointer = 0;
}

void post_slip_decoder(
    decoder* dec,
    uint8_t* msg,
    uint32_t size,
    void (*on_complete)(uint8_t* decoded, uint32_t size)) {
  int j = dec->pointer;
  for (int i = 0; i < size; i++) {
    if (msg[i] == ESC) {
      i++;
      if (msg[i] == ESC_END) {
        dec->buffer[j] = END;
      } else if (msg[i] == ESC_ESC) {
        dec->buffer[j] = ESC;
      }
    } else if (msg[i] == END) {
      on_complete(dec->buffer, j);
      j = -1;
    } else {
      dec->buffer[j] = msg[i];
    }
    j++;
  }
  dec->pointer = j;
}
