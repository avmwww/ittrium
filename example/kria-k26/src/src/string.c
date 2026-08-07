/**
 * Strict-align-safe memory routines for AArch64 baremetal.
 * Overrides newlib versions that may use unaligned accesses.
 */
#include <stddef.h>

void *memset(void *s, int c, size_t n)
{
  unsigned char *p = (unsigned char *)s;
  unsigned char v = (unsigned char)c;

  while (n--)
    *p++ = v;
  return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;

  while (n--)
    *d++ = *s++;
  return dst;
}

void *memmove(void *dst, const void *src, size_t n)
{
  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;

  if (d == s || n == 0)
    return dst;
  if (d < s) {
    while (n--)
      *d++ = *s++;
  } else {
    d += n;
    s += n;
    while (n--)
      *--d = *--s;
  }
  return dst;
}

int memcmp(const void *a, const void *b, size_t n)
{
  const unsigned char *p = (const unsigned char *)a;
  const unsigned char *q = (const unsigned char *)b;

  while (n--) {
    if (*p != *q)
      return (int)*p - (int)*q;
    p++;
    q++;
  }
  return 0;
}
