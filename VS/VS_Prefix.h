/*
 *  VS_Prefix.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 8/04/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#ifndef VS_PREFIX_H
#define VS_PREFIX_H


#if defined __APPLE__
#include "TargetConditionals.h"
#endif


#ifndef NULL
#define NULL (0L)
#endif //

#define BIT(x) (1<<x)

#ifdef UNUSED
//#elif defined(__GNUC__)
#elif defined(__cplusplus)
#define UNUSED (void)
//#define UNUSED(x) x __attribute__((unused))
//# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

// Define our 'vsTuneable' type, which lets us change tuning values at a breakpoint,
// while still compiling down to 'const' in release builds.
#if defined(_DEBUG)
#define vsTuneable static
#else
#define vsTuneable const
#endif

#define ENABLE_ASSERTS

#if defined(__cplusplus)

#include <string>
#include <stdlib.h>
#ifdef UNIX
#include <stdint.h>
#endif
#include <stdio.h>
#include <string.h>
#include <exception> // for std::bad_alloc
#include "VS/Math/VS_Math.h"
#include "VS/Utils/VS_Debug.h"
#include "VS/Utils/VS_String.h"
#include "VS/Utils/VS_Log.h"

void * MyMalloc(size_t size, const char*fileName, int lineNumber, int allocType = 1);	// 1 == Malloc type.  We can ignore this.  :)
void MyFree(void *p, int allocType = 1);

void* operator new (size_t size, const char *file, int line);
void* operator new[] (size_t size, const char *file, int line);
void operator delete (void *ptr, const char *file, int line) throw();
void operator delete[] (void *ptr, const char *file, int line) throw();
void operator delete (void *ptr) throw();
void operator delete[] (void *ptr) throw();

#define vsDelete(x) { if ( x ) { delete x; x = NULL; } }

#define vsDeleteArray(x) { if ( x ) { delete [] x; x = NULL; } }

#define INGAME_NEW new(__FILE__, __LINE__)
#define new INGAME_NEW

#define malloc(x) MyMalloc(x, __FILE__, __LINE__)
#define free(p) MyFree(p)


#if defined(_WIN32)

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;

#else // __GNUC__, __APPLE_CC__, and probably everything else?

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;

#endif	// normal Linux/BSD/etc section

#endif // __cplusplus

#endif // VS_PREFIX_H

