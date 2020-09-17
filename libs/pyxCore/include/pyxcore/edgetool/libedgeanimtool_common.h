#pragma once

#include <vector>
#include "pyxieMathutil.h"
//#include <edge_stdint.h>

//--------------------------------------------------------------------------------------------------

//#define EDGESTRINGIFY_(x) #x
//#define EDGESTRINGIFY(x) EDGESTRINGIFY_(x)
//#define EDGEERROR_F() { throw std::exception(__FILE__ "(" EDGESTRINGIFY(__LINE__) ")"); }
//#define EDGEERROR(x) { if (!(x)) EDGEERROR_F() }

//--------------------------------------------------------------------------------------------------

namespace pyxie
{

	class pyxieMemoryWriter;
	class BitStream;

	//--------------------------------------------------------------------------------------------------

	// callback type for adding custom data to an Edge anim/skel resource file
	typedef void(*CustomDataCallback) (pyxieMemoryWriter& writer, void* customData);

	//--------------------------------------------------------------------------------------------------

	// custom data table - allows many, independent custom data sections using hash-offset pairs
	class CustomDataTableEntry
	{
	public:
		virtual	unsigned int	GetHashValue() const = 0;
		virtual	unsigned int	GetDataAlignment() const = 0;

		virtual void			ExportData(pyxieMemoryWriter& fileWriter) const = 0;
	};

	struct CustomDataTable
	{
		std::vector<CustomDataTableEntry*>	m_CustomDataEntries;
	};

	void ExportCustomDataTable(pyxieMemoryWriter& fileWriter, void* customData);

	//--------------------------------------------------------------------------------------------------

	//struct Float4 // workaround for STL containers that can't use a float[4]
	//{
	//public:
	//	inline const float& operator[](unsigned int i) const { return m_data[i]; }
	//	inline float& operator[](unsigned int i) { return m_data[i]; }
	//	float m_data[4];
	//};

	//struct Joint
	//{
	//	Float4  m_rotation;             // joint rotation (quaternion)
	//	Float4  m_translation;          // joint translation 
	//	Float4  m_scale;                // joint scale
	//};

	//--------------------------------------------------------------------------------------------------

	//Joint   MatrixToJoint(const float* pMatrix4x4);
	uint64_t CompressQuat(const Vec4& q);
	void    DecompressQuat(Vec4& pRes, uint64_t q);
	float	QuatAbsError(const Vec4& pQa, const Vec4& pQb);
	float	VecAbsError(const Vec4& pVa, const Vec4& pVb);
	//Float4  Reverse(const Float4& a);
	void    WriteBitStreamToFile(BitStream& bitStream, pyxieMemoryWriter& fileWriter);

//	inline unsigned char Reverse(unsigned char v)
//	{
//		return v;
//	}

//	inline unsigned short Reverse(unsigned short v)
//	{
//		return ((v & 0xFF00) >> 8) | ((v & 0x00FF) << 8);
//	}

//	inline unsigned Reverse(unsigned v)
//	{
//		return ((v & 0xFF000000) >> 24) | ((v & 0x00FF0000) >> 8) | ((v & 0x0000FF00) << 8) | ((v & 0x000000FF) << 24);
//	}

//	inline signed Reverse(signed v)
//	{
//		return ((((unsigned)v) & 0xFF000000) >> 24) | ((((unsigned)v) & 0x00FF0000) >> 8) | ((((unsigned)v) & 0x0000FF00) << 8) | ((((unsigned)v) & 0x000000FF) << 24);
//	}

//	inline float Reverse(float a)
//	{
//		union
//		{
//			unsigned int u32;
//			float        f32;
//		} safe;
//		safe.f32 = a;
//		safe.u32 = Reverse(safe.u32);
//		return safe.f32;
//	}

	//--------------------------------------------------------------------------------------------------
}


