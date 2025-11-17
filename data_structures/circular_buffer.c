#include "autoconf.h"
#include "circular_buffer.h"

int add_to_cbuffer(cbuffer_t * buf, char c, bool is_uppercase){

  size_t next_top = (buf -> top + 1) % buf -> size;

  if (next_top == buf -> bot) {
    return -1;
  }
  
  buf -> array[buf -> top] = c - (is_uppercase && c <= 122 && c >= 97 ? 32 : 0);
  buf -> top = next_top;
  return 0;
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
