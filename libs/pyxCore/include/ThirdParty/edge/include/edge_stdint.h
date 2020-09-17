/* SCE CONFIDENTIAL
 * PlayStation(R)Edge 1.1.1
 * Copyright (C) 2007 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */
#ifndef EDGE_STDINT_H
#define EDGE_STDINT_H

/* 
 * Workaround for PC tools. MSVC is still not C99 compliant.
 */

#if defined(_MSC_VER)

	// gcm_tool.h simply doesn't compile under Visual Studio 2003 and earlier,
	// so we include the relevant typedefs directly. 
	typedef signed char			int8_t;
	typedef signed short		int16_t;
	typedef signed long long	int64_t;
	typedef unsigned char		uint8_t;
	typedef unsigned short		uint16_t;
#ifndef def_uint32_t
	typedef signed int			int32_t;
	typedef unsigned int		uint32_t;
#define def_uint32_t
#endif
	typedef unsigned long long	uint64_t;

	// Non-standard types representing a 32-bit pointer.
	// Used in structures defined in both the tools (32- or 64-bit) and the runtime
	// (always 32-bit), so that these structures have the same size in both environments.
	typedef uint32_t			voidptr32_t;
	typedef uint32_t			uint16ptr32_t;
	typedef uint32_t			uint32ptr32_t;

#if defined(WIN64) || defined(_M_X64)
	typedef int64_t				intptr_t;
	typedef uint64_t			uintptr_t;
#else

#ifndef _INTPTR_T_DEFINED
#define _INTPTR_T_DEFINED
typedef long intptr_t;
#endif

#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
typedef unsigned long uintptr_t;
#endif

#endif // !defined(WIN64) || !defined(_M_X64)

#else	//PPU, SPU, other?

	#include <stdint.h>

	// Non-standard type representing a 32-bit pointer (see above)
	typedef void*				voidptr32_t;
	typedef uint16_t*			uint16ptr32_t;
	typedef uint32_t*			uint32ptr32_t;

#endif

#endif // EDGE_STDINT_H
