/**
 * lwIP compiler / platform abstraction for ittrium
 */
#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Avoid lwIP redefining ssize_t (already in newlib sys/types.h) */
#ifndef SSIZE_MAX
#define SSIZE_MAX LONG_MAX
#endif
#define LWIP_NO_UNISTD_H 1

#ifdef BYTE_ORDER
/* keep platform BYTE_ORDER if already defined */
#else
# if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && \
     (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#  define BYTE_ORDER BIG_ENDIAN
# else
#  define BYTE_ORDER LITTLE_ENDIAN
# endif
#endif

typedef uint8_t   u8_t;
typedef int8_t    s8_t;
typedef uint16_t  u16_t;
typedef int16_t   s16_t;
typedef uint32_t  u32_t;
typedef int32_t   s32_t;

typedef uintptr_t mem_ptr_t;

#define LWIP_RAND()  ((u32_t)rand())

#define PACK_STRUCT_STRUCT  __attribute__((packed))
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do { printf x; } while (0)
#endif

#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) do { \
  printf("Assertion \"%s\" failed at line %d in %s\n", \
         x, __LINE__, __FILE__); \
  for (;;) { } \
} while (0)
#endif

#define LWIP_CONST_CAST(target_type, val) ((target_type)((ptrdiff_t)val))

#endif /* LWIP_ARCH_CC_H */
