#include "core/shortcut/IShortcut.h"

NS_IGE_BEGIN

IShortcut::IShortcut() {
	ShortcutRegistration::getInstance()->addObject(this);
	autorelease();
	retain();
}

IShortcut::~IShortcut() {
	// unregisterShortcut();
}

void IShortcut::unregisterShortcut() { }


ShortcutRegistration* ShortcutRegistration::s_singleInstance = nullptr;
ShortcutRegistration* ShortcutRegistration::getInstance() {
	if (s_singleInstance == nullptr)
	{
		s_singleInstance = new ShortcutRegistration();
	}
	return s_singleInstance;
}

ShortcutRegistration::ShortcutRegistration() {
	_managedObjectArray.reserve(150);

	autorelease();
	retain();
}

ShortcutRegistration::~ShortcutRegistration() {
	clear();
}

void ShortcutRegistration::addObject(IShortcut* object) {
	_managedObjectArray.push_back(object);
}

void ShortcutRegistration::clear() {
	std::vector<IShortcut*> releasings;
	releasings.swap(_managedObjectArray);
	for (const auto& obj : releasings)
	{
		obj->release();
	}
}

bool ShortcutRegistration::contains(IShortcut* object) const
{
	for (const auto& obj : _managedObjectArray)
	{
		if (obj == object)
			return true;
	}
	return false;
}

void ShortcutRegistration::registerShortcuts() {
	for (const auto& obj : _managedObjectArray)
	{
		obj->registerShortcut();
	}
}

NS_IGE_END
