#include "core/Ref.h"
#include "core/AutoReleasePool.h"

NS_IGE_BEGIN
	
Ref::Ref() : _referenceCount(0) {}

Ref::~Ref() { }

void Ref::retain()
{
	++_referenceCount;
}

void Ref::release()
{
	--_referenceCount;
	
	//Only destroy whenever nothing reference to this obj
	if (_referenceCount == 0)
		delete this;
}

Ref* Ref::autorelease()
{
	//Add this obj to AutoReleasePool for safe release
	PoolManager::getInstance()->getCurrentPool()->addObject(this);
	return this;
}

unsigned int Ref::getReferenceCount() const
{
	return _referenceCount;
}

NS_IGE_END