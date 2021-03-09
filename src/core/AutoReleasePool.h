#pragma once
#ifndef __AUTORELEASEPOOL_H__
#define __AUTORELEASEPOOL_H__

#include <vector>
#include "core/Ref.h"

NS_IGE_BEGIN

class AutoReleasePool
{
public:
	AutoReleasePool();
	~AutoReleasePool();

	void addObject(Ref* object);
	void clear();
	bool contains(Ref* object) const;

private:
	std::vector<Ref*> _managedObjectArray;
};

class PoolManager
{
public:
	static PoolManager* getInstance();
	static void destroyInstance();

	AutoReleasePool* getCurrentPool() const;

	bool isObjectInPools(Ref* obj) const;

	friend class AutoReleasePool;

private:
	PoolManager();
	~PoolManager();

	void push(AutoReleasePool* pool);
	void pop();

	static PoolManager* s_singleInstance;

	std::vector<AutoReleasePool*> _releasePoolStack;
};
NS_IGE_END

#endif