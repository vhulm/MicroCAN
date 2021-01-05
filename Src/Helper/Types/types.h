#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>

#ifndef TRUE
#define TRUE 1u
#endif

#ifndef FALSE
#define FALSE 0u
#endif

typedef unsigned char boolean; /*TRUE/FALSE*/
typedef signed char sint8;     /*-128 .. +127*/
typedef unsigned char uint8;   /*0 .. 255*/
typedef signed short sint16;   /*-32768 .. +32767*/
typedef unsigned short uint16; /*0 .. 65535*/
typedef signed int sint32;     /*-2147483648 .. +2147483647*/
typedef unsigned int uint32;   /*0 .. 4294967295*/
typedef signed long long sint64;
typedef unsigned long long uint64;

typedef float float32;
typedef double float64;

#ifndef NULL
#ifdef __cplusplus
#define NULL (0u)
#else
#define NULL ((void *)0)
#endif
#endif

#endif /* TYPES_H */
