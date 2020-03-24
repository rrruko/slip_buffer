#include <stdint.h>

#define END 0xC0
#define ESC 0xDB
#define ESC_END 0xDC
#define ESC_ESC 0xDD

typedef struct {
  uint8_t* buffer;
  uint32_t size;
  uint32_t pointer;
} decoder;

void post_slip_decoder(
    decoder* dec,
    uint8_t* msg,
    uint32_t size,
    void (*on_complete)(uint8_t* decoded, uint32_t size));
