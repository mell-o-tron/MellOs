#include "circular_buffer.h"
// #ifdef VGA_VESA
// #include "../drivers/vesa/vesa_text.h"
// #else
// #include "../drivers/vga_text.h"
// #endif
// #include "../utils/conversions.h"

void add_to_cbuffer(cbuffer_t * buf, char c, bool is_uppercase){
  
  buf -> array[buf -> top] = c - (is_uppercase && c <= 122 && c >= 97 ? 32 : 0);
  buf -> top = ((buf -> top + 1) % buf -> size);
}

char get_from_cbuffer(cbuffer_t * buf){
  if (buf -> bot == buf -> top)
    return 0;

  char res = buf -> array[buf -> bot];
  buf -> bot = ((buf -> bot + 1) % buf -> size);

  return res;
}

void rem_from_cbuffer(cbuffer_t * buf){
  if (buf -> bot != buf -> top)
    buf -> bot = ((buf -> bot + 1) % buf -> size);
}
