#include "core/panels/components/navigation/NavigableEditorComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/Navigable.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavigableEditorComponent::NavigableEditorComponent() {
    m_navigableGroup = nullptr;
}

NavigableEditorComponent::~NavigableEditorComponent()
{
    if (m_navigableGroup) {
        m_navigableGroup->removeAllWidgets();
        m_navigableGroup->removeAllPlugins();
    }
    m_navigableGroup = nullptr;
}

bool NavigableEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<Navigable*>(comp);
}

void NavigableEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_navigableGroup == nullptr) {
        m_navigableGroup = m_group->createWidget<Group>("Navigable", false);
    }
    drawNavigable();

    EditorComponent::redraw();
}

void NavigableEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_navigableGroup = m_group->createWidget<Group>("Navigable", false);

    drawNavigable();
}

void NavigableEditorComponent::drawNavigable()
{
    if (m_navigableGroup == nullptr)
        return;
    m_navigableGroup->removeAllWidgets();

    auto navigable = m_targetObject->getComponent<Navigable>();
    if (navigable == nullptr)
        return;

    auto column = m_navigableGroup->createWidget<Columns<3>>();
    column->createWidget<CheckBox>("Enable", navigable->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto navigable = m_targetObject->getComponent<Navigable>();
        navigable->setEnabled(val);
        });
    column->createWidget<CheckBox>("Recursive", navigable->isRecursive())->getOnDataChangedEvent().addListener([this](bool val) {
        auto navigable = m_targetObject->getComponent<Navigable>();
        navigable->setRecursive(val);
        });
}
NS_IGE_END