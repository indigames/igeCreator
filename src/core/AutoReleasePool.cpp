#include "core/AutoReleasePool.h"

NS_IGE_BEGIN

AutoReleasePool::AutoReleasePool()
{
	_managedObjectArray.reserve(150);
	PoolManager::getInstance()->push(this);
}

AutoReleasePool::~AutoReleasePool()
{
	clear();
	PoolManager::getInstance()->pop();
}

void AutoReleasePool::addObject(Ref* object) {
	_managedObjectArray.push_back(object);
}

void AutoReleasePool::clear() {
	std::vector<Ref*> releasings;
	releasings.swap(_managedObjectArray);
    releasings.clear();
}

bool AutoReleasePool::contains(Ref* object) const
{
	for (const auto& obj : _managedObjectArray)
	{
		if (obj == object)
			return true;
	}
	return false;
}


PoolManager* PoolManager::s_singleInstance = nullptr;

PoolManager* PoolManager::getInstance()
	{
		if (s_singleInstance == nullptr)
		{
			s_singleInstance = new PoolManager();
			//! Add AutoReleasePool
			new AutoReleasePool();
		}
		return s_singleInstance;
	}

void PoolManager::destroyInstance()
{
	delete s_singleInstance;
	s_singleInstance = nullptr;
}

PoolManager::PoolManager()
{
	_releasePoolStack.reserve(10);
}

PoolManager::~PoolManager()
{
	while (!_releasePoolStack.empty())
	{
		AutoReleasePool* pool = _releasePoolStack.back();

		delete pool;
	}
}

AutoReleasePool* PoolManager::getCurrentPool() const
{
	return _releasePoolStack.back();
}

bool PoolManager::isObjectInPools(Ref* obj) const
{
	for (const auto& pool : _releasePoolStack)
	{
		if (pool->contains(obj))
			return true;
	}
	return false;
}

void PoolManager::push(AutoReleasePool* pool)
{
	_releasePoolStack.push_back(pool);
}

void PoolManager::pop()
{
	if (_releasePoolStack.empty()) return;
	_releasePoolStack.pop_back();
}

NS_IGE_END
