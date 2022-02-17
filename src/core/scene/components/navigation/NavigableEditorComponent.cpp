#include "core/scene/components/navigation/NavigableEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/Navigable.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavigableEditorComponent::NavigableEditorComponent() {
    m_navigableGroup = nullptr;
}

NavigableEditorComponent::~NavigableEditorComponent() {
    m_navigableGroup = nullptr;
}

void NavigableEditorComponent::onInspectorUpdate() {
    drawNavigable();
}

void NavigableEditorComponent::drawNavigable()
{
    if (m_navigableGroup == nullptr)
        m_navigableGroup = m_group->createWidget<Group>("Navigable", false);;
    m_navigableGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_navigableGroup->createWidget<CheckBox>("Recursive", comp->getProperty<bool>("recursive", false))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("recursive", val);
    });
}
NS_IGE_END