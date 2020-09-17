#pragma once
#include <stdint.h>

namespace pyxie
{

	class BitStream
	{
	public:
		BitStream(void);
		~BitStream(void);

		void            Write(uint64_t data, unsigned int numBits);
		unsigned int    GetBufferSize();
		unsigned char*  GetBuffer();
		void            ByteAlign();

	private:

		void            AddByte(unsigned char a);

		unsigned char*  m_pBuffer;
		unsigned char*  m_pBufferPos;
		unsigned int    m_curBitCount;
		uint64_t        m_curByte;
		unsigned int    m_maxBufferSize;
	};

	inline unsigned char* BitStream::GetBuffer()
	{
		return m_pBuffer;
	}

	inline unsigned int BitStream::GetBufferSize()
	{
		return (unsigned int)(m_pBufferPos - m_pBuffer);
	}



}