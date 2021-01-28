#include "core/shortcut/IShortcut.h"

NS_IGE_BEGIN

IShortcut::IShortcut() {
	IShortcutRegistration::getInstance()->addObject(this);
}

void IShortcut::unregisterShortcut() { }

IShortcut::~IShortcut() {
	unregisterShortcut();
}


IShortcutRegistration* IShortcutRegistration::s_singleInstance = nullptr;
IShortcutRegistration* IShortcutRegistration::getInstance() {
	if (s_singleInstance == nullptr)
	{
		s_singleInstance = new IShortcutRegistration();
	}
	return s_singleInstance;
}

IShortcutRegistration::IShortcutRegistration() {
	_managedObjectArray.reserve(150);
}

IShortcutRegistration::~IShortcutRegistration() {
	clear();
}

void IShortcutRegistration::addObject(IShortcut* object) {
	_managedObjectArray.push_back(object);
}

void IShortcutRegistration::clear() {
	std::vector<IShortcut*> releasings;
	releasings.swap(_managedObjectArray);
	for (const auto& obj : releasings)
	{
		obj->release();
	}
}

bool IShortcutRegistration::contains(IShortcut* object) const
{
	for (const auto& obj : _managedObjectArray)
	{
		if (obj == object)
			return true;
	}
	return false;
}

void IShortcutRegistration::registerShortcuts() {
	for (const auto& obj : _managedObjectArray)
	{
		obj->registerShortcut();
	}
}

NS_IGE_END