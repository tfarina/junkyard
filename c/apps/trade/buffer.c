#include "buffer.h"

#include <stdlib.h>
#include <string.h>

void buffer_add(buffer_t *buf, char *data, size_t size)
{
  buf->data = (char *)realloc(buf->data, buf->length + size + 1);
  if (buf->data) {
    memcpy(&(buf->data[buf->length]), data, size);
    buf->length += size;
    buf->data[buf->length] = 0;
  }
}
