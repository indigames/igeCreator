#include "core/panels/EditorComponent.h"
#include "core/panels/InspectorEditor.h"


#include <core/layout/Group.h>


NS_IGE_BEGIN

EditorComponent::EditorComponent() 
	: m_component(nullptr),
	m_group(nullptr),
	m_bisDirty(false)
{	
}

EditorComponent::~EditorComponent() {
	m_component = nullptr;
	m_group = nullptr;
}

bool EditorComponent::isSafe(Component* comp) {
	return false;
}

void EditorComponent::draw(std::shared_ptr<Group> group) {
	m_group = group;
	onInspectorUpdate();
}

void EditorComponent::redraw() {
	m_bisDirty = false;
}

bool EditorComponent::setComponent(Component* component) 
{
	if (isSafe(component)) {
		m_component = component;
		return true;
	}
	return false;
}

Component* EditorComponent::getComponent()
{
	return m_component;
}

NS_IGE_END