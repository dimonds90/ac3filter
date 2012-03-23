#define INITGUID
#include <objbase.h>
#include <initguid.h>
#include "guids.h"

void cr2crlf(char *_buf, size_t _size)
{
  int cnt = 0;

  char *src;
  char *dst;

  src = _buf;
  dst = _buf + _size;
  while (*src && src < dst)
  {
    if (*src == '\n')
      cnt++;
    src++;
  }

  dst = src + cnt;
  if (dst > _buf + _size)
    dst = _buf + _size;

  while (src != dst)
  {
    *dst-- = *src--;
    if (src[1] == '\n')
      *dst-- = '\r';
  }
}

