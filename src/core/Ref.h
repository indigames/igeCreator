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
    virtual bool init();
public:
	~Ref();
protected:
	unsigned int _referenceCount;
};

#define CREATE_FUNC(__TYPE__) \
static __TYPE__* create() \
{ \
    __TYPE__ *pRet = new __TYPE__(); \
    if (pRet && pRet->init()) \
    { \
        pRet->autorelease(); \
        return pRet; \
    } \
    else \
    { \
        delete pRet; \
        pRet = nullptr; \
        return nullptr; \
    } \
}

NS_IGE_END
#endif