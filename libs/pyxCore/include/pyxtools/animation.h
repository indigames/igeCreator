#pragma once

//#include "edgetool/libedgeanimtool_animation.h"
#include "pyxieColladaCollection.h"
#include "pyxieEditableFigure.h"

namespace pyxie
{
	bool ExtractAnimation(pyxieColladaCollection& collada,
		Animation& outputAnim,
		const Skeleton& bindSkeleton,
		const Skeleton& animSkeleton,
		bool computePeriod);

	bool ExtractSkeleton(pyxieColladaCollection& collada, Skeleton& skeleton);

	float PYXIE_EXPORT ComputePeriod(const std::vector<float>& keyTimes, double gcd);
	bool PYXIE_EXPORT GenerateAdditiveAnimation(Animation& outAnimation, const Animation& baseAnimation, const Animation& animation, const Skeleton& bindSkeleton, bool baseUsesFirstFrame);
	void FindUserChanelAnimation(const FCDocument* pDocument, const std::vector<const FCDSceneNode*>& joints, std::vector<UserChannelInfo>& outInfos);
	void GetUserChanelAnimation(const FCDocument* pDocument, const std::vector<const FCDSceneNode*>& joints, std::vector<UserChannelAnimation>& outAnimations, float startTime, float endTime);
	bool GetUserAnimeTime(const FCDocument* pDocument, float& outStartTime, float& outEndTime);
}
