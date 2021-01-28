#pragma one
#ifndef __CORE_REF_H__
#define __CORE_REF_H__

#include "core/Macros.h"

NS_IGE_BEGIN

class Ref
{
public:

	void retain();

	void release();

	Ref* autorelease();

	unsigned int getReferenceCount() const;

protected:
	Ref();
public:
	~Ref();
protected:
	unsigned int _referenceCount;
};

NS_IGE_END
#endif