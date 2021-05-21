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
	~EditorComponent();

	virtual void setTargetObject(SceneObject* obj);
	virtual bool setComponent(Component* component);
	virtual Component* getComponent();
	virtual void draw(std::shared_ptr<Group> group);
	virtual void redraw();

	void dirty() { m_bisDirty = true; }
	bool isDirty() { return m_bisDirty; }
	void setDirty(bool value) { m_bisDirty = value; }
protected:
	virtual bool isSafe(Component* comp);
	virtual void onInspectorUpdate() = 0;

protected:
	Component* m_component = nullptr;
	std::shared_ptr<Group> m_group = nullptr;
	SceneObject* m_targetObject = nullptr;
	bool m_bisDirty;
};

#define INSPECTOR_WATCH(COMPONENT, TYPE, VALUE) \
do { \
	auto typeId = std::type_index(typeid(TYPE)); \
	auto address = std::addressof(VALUE); \
	Editor::getCanvas()->getInspector()->getInspectorEditor()->addWatcherValue(COMPONENT->getInstanceId(), typeId, (void*)address, VALUE); \
}\
while(0) \

NS_IGE_END

#endif // ! 