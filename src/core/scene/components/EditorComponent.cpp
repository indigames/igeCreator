#include "core/scene/components/EditorComponent.h"
#include "core/panels/InspectorEditor.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>
#include <core/widgets/CheckBox.h>

NS_IGE_BEGIN

EditorComponent::EditorComponent() 
	: m_group(nullptr),
	m_bisDirty(false)
{	
}

EditorComponent::~EditorComponent() {
	m_group = nullptr;
	m_component.reset();	
}

void EditorComponent::draw(std::shared_ptr<Group> group) {
	m_group = group;
	if (m_group != nullptr) {		
		m_group->createWidget<CheckBox>("Enable", getComponent<CompoundComponent>()->getProperty("enabled", true))->getOnDataChangedEvent().addListener([this](bool val) {
			getComponent<CompoundComponent>()->setProperty("enabled", val);
		});		
		onInspectorUpdate();
	}
}

void EditorComponent::redraw() {
	if (isDirty()) {
		if (m_group != nullptr) {
			onInspectorUpdate();
		}
		setDirty(false);
	}
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