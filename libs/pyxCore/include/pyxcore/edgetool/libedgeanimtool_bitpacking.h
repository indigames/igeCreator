#pragma once

#include <assert.h>
#include <math.h>
#include <vector>
#include "pyxieMathutil.h"

namespace pyxie
{

	class Animation;
	class Skeleton;
	struct FrameSet;
	class BitStream;

	const unsigned kKeyframePackingSpecSizeBits = 32;

	struct ComponentPackingSpec
	{
		ComponentPackingSpec() : m_numSignBits(1), m_numExponentBits(8), m_numMantissaBits(23) {};
		ComponentPackingSpec(unsigned s, unsigned e, unsigned m) : m_numSignBits(s), m_numExponentBits(e), m_numMantissaBits(m) {};

		unsigned	m_numSignBits;
		unsigned	m_numExponentBits;
		unsigned	m_numMantissaBits;

		unsigned	GetNumBits() const { return m_numSignBits + m_numExponentBits + m_numMantissaBits; }
		unsigned	Encode(float flt) const;
		float		Decode(unsigned bits) const;
	};

	struct KeyframePackingSpec
	{
		KeyframePackingSpec() : m_recomputeComponentIdx(0) {};

		unsigned				m_recomputeComponentIdx;
		ComponentPackingSpec	m_componentSpecs[3];

		unsigned				GetNumBits() const { return m_componentSpecs[0].GetNumBits() + m_componentSpecs[1].GetNumBits() + m_componentSpecs[2].GetNumBits(); }
		unsigned				GetPackedSpec();
	};

	bool FindBestPackingSpecRotations(std::vector<KeyframePackingSpec>& animSpecs, Animation& animation, std::vector<float>& tolerances);
	bool FindBestPackingSpecTranslations(std::vector<KeyframePackingSpec>& animSpecs, Animation& animation, std::vector<float>& tolerances);
	bool FindBestPackingSpecScales(std::vector<KeyframePackingSpec>& animSpecs, Animation& animation, std::vector<float>& tolerances);
	bool FindBestPackingSpecUserChannels(std::vector<KeyframePackingSpec>& animSpecs, Animation& animation, std::vector<float>& tolerances);

	bool FindBestPackingSpecConstRotations(KeyframePackingSpec& spec, Animation& animation, float tolerance);
	bool FindBestPackingSpecConstTranslations(KeyframePackingSpec& spec, Animation& animation, float tolerance);
	bool FindBestPackingSpecConstScales(KeyframePackingSpec& spec, Animation& animation, float tolerance);
	bool FindBestPackingSpecConstUserChannels(KeyframePackingSpec& spec, Animation& animation, float tolerance);

	void BitCompressQuat(BitStream& bitStream, const Vec4& q, const KeyframePackingSpec& spec);
	void BitCompressVec3(BitStream& bitStream, const Vec4& v, const KeyframePackingSpec& spec);
	void BitCompressFloat(BitStream& bitStream, float v, const KeyframePackingSpec& spec);

}