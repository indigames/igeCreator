///////////////////////////////////////////////////////////////
//Pyxie game engine
//
//  Copyright Kiharu Shishikura 2019. All rights reserved.
///////////////////////////////////////////////////////////////
#pragma once

#include <anim/edgeanim_structs.h>
#if defined _WIN32			//WIN32
#define __attribute__(x)
#endif
typedef EdgeAnimSkeleton SkeletonSet;
typedef EdgeAnimAnimation AnimationSet;
typedef EdgeAnimBlendBranch BlendBranch;
typedef EdgeAnimBlendLeaf BlendLeaf;
