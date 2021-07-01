#pragma once
#ifndef __EDITOR_COMPONENT_H__
#define __EDITOR_COMPONENT_H__


#include "core/Macros.h"

#include <string>
#include <typeinfo>
#include <typeindex>

#include "scene/SceneObject.h"
#include <core/layout/Group.h>
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class EditorComponent
{
public:
	EditorComponent();
	virtual ~EditorComponent();

	virtual bool setComponent(std::shared_ptr<Component> component);
	virtual void draw(std::shared_ptr<Group> group);
	virtual void redraw();

	void dirty() { m_bisDirty = true; }
	bool isDirty() { return m_bisDirty; }
	void setDirty(bool value) { m_bisDirty = value; }

	std::shared_ptr<Component>& getComponent() { return m_component; }

	//! Check if component type is safe to cast
	template <typename T>
	inline bool isSafe(const std::shared_ptr<Component>& component);

	//! Get component by type
	template <typename T>
	inline std::shared_ptr<T> getComponent();

protected:
	virtual void onInspectorUpdate() = 0;

protected:
	std::shared_ptr<Component> m_component = nullptr;
	std::shared_ptr<Group> m_group = nullptr;
	bool m_bisDirty;
};

//! Check if component type is safe to cast
template <typename T>
inline bool EditorComponent::isSafe(const std::shared_ptr<Component>& component)
{
	return std::dynamic_pointer_cast<T>(component) != nullptr;
}

//! Get component by type
template <typename T>
inline std::shared_ptr<T> EditorComponent::getComponent()
{
	return std::dynamic_pointer_cast<T>(m_component);
}

#define INSPECTOR_WATCH(COMPONENT, TYPE, VALUE) \
do { \
	auto typeId = std::type_index(typeid(TYPE)); \
	auto address = std::addressof(VALUE); \
	Editor::getCanvas()->getInspector()->getInspectorEditor()->addWatcherValue(COMPONENT->getInstanceId(), typeId, (void*)address, VALUE); \
}\
while(0) \

NS_IGE_END

#endif // ! 