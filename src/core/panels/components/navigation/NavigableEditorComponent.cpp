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
    m_navigableGroup = nullptr;
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

    auto navigable = getComponent<Navigable>();
    if (navigable == nullptr)
        return;

    auto column = m_navigableGroup->createWidget<Columns<3>>();
    column->createWidget<CheckBox>("Enable", navigable->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto navigable = getComponent<Navigable>();
        navigable->setEnabled(val);
        });
    column->createWidget<CheckBox>("Recursive", navigable->isRecursive())->getOnDataChangedEvent().addListener([this](bool val) {
        auto navigable = getComponent<Navigable>();
        navigable->setRecursive(val);
        });
}
NS_IGE_END