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
		
	//! Draw
	virtual void draw(std::shared_ptr<Group> group);
	virtual void redraw();

	//! Dirty flag
	bool isDirty() { return m_bisDirty; }
	void setDirty(bool value = true) { m_bisDirty = value; }

	//! Get component
	std::shared_ptr<Component>& getComponent() { return m_component; }
	virtual bool setComponent(std::shared_ptr<Component> component);

	//! Check if component type is safe to cast
	template <typename T>
	inline bool isSafe(const std::shared_ptr<Component>& component);

	//! Get component by type
	template <typename T>
	inline std::shared_ptr<T> getComponent();

protected:
	//! Internal inspector update
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
	return (component != nullptr) && std::dynamic_pointer_cast<T>(component) != nullptr;
}

//! Get component by type
template <typename T>
inline std::shared_ptr<T> EditorComponent::getComponent()
{
	return (m_component != nullptr) ? std::dynamic_pointer_cast<T>(m_component) : nullptr;
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