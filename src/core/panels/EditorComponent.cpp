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
	m_group = nullptr;
	m_component = nullptr;	
}

void EditorComponent::draw(std::shared_ptr<Group> group) {
	m_group = group;
	onInspectorUpdate();
}

void EditorComponent::redraw() {
	m_bisDirty = false;
}

bool EditorComponent::setComponent(std::shared_ptr<Component> component)
{
	if (isSafe<Component>(component)) {
		m_component = component;
		return true;
	}
	return false;
}

NS_IGE_END