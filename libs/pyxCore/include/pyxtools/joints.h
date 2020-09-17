#pragma once

#include <vector>

class FCDSceneNode;
namespace pyxie
{
	void FindSkeletonJoints(const FCDSceneNode* pNode, std::vector<const FCDSceneNode*>& joints);
}
