#include <stdint.h>

#define END 0xC0
#define ESC 0xDB
#define ESC_END 0xDC
#define ESC_ESC 0xDD

typedef struct {
  uint8_t* buffer;
  uint32_t size;
  uint32_t pointer;
} encoder;

void init_slip_encoder(encoder* enc, uint8_t* buf, uint32_t size);
void post_slip_encoder(encoder* enc, uint8_t* msg, uint32_t size);
void terminate_message_slip_encoder(encoder* enc);
