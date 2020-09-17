/* SCE CONFIDENTIAL
 * PlayStation(R)Edge 1.1.1
 * Copyright (C) 2007 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 */

#ifndef __EDGE_ANIM_PPU_H__
#define __EDGE_ANIM_PPU_H__

#include "anim/edgeanim_structs_ps3.h"

#ifdef __cplusplus
extern "C" 
{
#endif // __cplusplus

// Initialise/finalize
void edgeAnimPpuInitialize(EdgeAnimPpuContext* ppuContext, unsigned int numSpus, int spuExternalStorageMask = 0, size_t sizeExternalStoragePerSpu = 0, void* externalStorageBlock = 0);
void edgeAnimPpuFinalize(EdgeAnimPpuContext* ppuContext);
size_t edgeAnimComputeExternalStorageSize( unsigned int numSpus, int spuExternalStorageMask, size_t sizeExternalStoragePerSpu);

// Skeleton
int edgeAnimSkeletonGetJointIndexByName(const EdgeAnimSkeleton* skeleton, const char* jointName);
int edgeAnimSkeletonGetJointIndexByHash(const EdgeAnimSkeleton* skeleton, unsigned int jointHash);
int edgeAnimSkeletonGetUserChannelIndexByName(const EdgeAnimSkeleton* skeleton, const char* userChannelNodeName, const char* userChannelName);
int edgeAnimSkeletonGetUserChannelIndexByHash(const EdgeAnimSkeleton* skeleton, unsigned int userChannelNodeNameHash, unsigned int userchannelNameHash);

// Evaluate a single joint/user channel on ppu (slow)
void edgeAnimEvaluateJoint(EdgeAnimJointTransform* outputJoint, const EdgeAnimAnimation* anim, const EdgeAnimSkeleton* skel, uint32_t jointIndex, float evalTime);
float edgeAnimEvaluateUserChannel(const EdgeAnimAnimation* anim, const EdgeAnimSkeleton* skel, uint32_t channelIndex, float evalTime);

// External joint weights - override built-in joint weights
void edgeAnimSetExternalJointWeights(EdgeAnimAnimation* animation, uint8_t* jointWeights);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __EDGE_ANIM_PPU_H__
