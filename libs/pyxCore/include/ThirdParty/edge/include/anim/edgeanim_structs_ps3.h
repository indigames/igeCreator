/* SCE CONFIDENTIAL
 * PlayStation(R)Edge 1.1.1
 * Copyright (C) 2007 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef __EDGE_ANIM_STRUCTS_PS3_H__
#define __EDGE_ANIM_STRUCTS_PS3_H__

#if !defined(__PPU__) && !defined(__SPU__) && !defined(__SNC__) && !defined(__ORBIS__)
#include "vectormath_aos.h"
#else
#include <vectormath_aos.h>
#endif

#include "anim/edgeanim_macros.h"
#include "anim/edgeanim_structs.h"

// Joint transform
// Note: this structure is declared in a separate file to avoid adding a dependency on vectormath to tools

#if defined(__PPU__) || defined(__SPU__)
#define VECTORMATH_AOS Vectormath::Aos
#elif defined(__SNC__) || defined(__ORBIS__)
#define VECTORMATH_AOS sce::Vectormath::Simd::Aos
#else
#define VECTORMATH_AOS Vectormath::Aos
#endif

typedef struct EDGE_ALIGNED(16) EdgeAnimJointTransform
{
	VECTORMATH_AOS::Quat		rotation;
	VECTORMATH_AOS::Point3		translation;
	VECTORMATH_AOS::Vector4		scale;
} EdgeAnimJointTransform;

#endif // __EDGE_ANIM_STRUCTS_PS3_H__

