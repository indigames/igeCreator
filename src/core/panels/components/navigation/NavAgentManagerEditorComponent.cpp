#include "core/panels/components/navigation/NavAgentManagerEditorComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavAgentManager.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavAgentManagerEditorComponent::NavAgentManagerEditorComponent() {
    m_navAgentManagerGroup = nullptr;
}

NavAgentManagerEditorComponent::~NavAgentManagerEditorComponent()
{
    if (m_navAgentManagerGroup) {
        m_navAgentManagerGroup->removeAllWidgets();
        m_navAgentManagerGroup->removeAllPlugins();
    }
    m_navAgentManagerGroup = nullptr;
}

bool NavAgentManagerEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<NavAgentManager*>(comp);
}

void NavAgentManagerEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_navAgentManagerGroup == nullptr) {
        m_navAgentManagerGroup = m_group->createWidget<Group>("NavAgentManager", false);
    }
    drawNavAgentManager();

    EditorComponent::redraw();
}

void NavAgentManagerEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_navAgentManagerGroup = m_group->createWidget<Group>("NavAgentManager", false);

    drawNavAgentManager();
}

void NavAgentManagerEditorComponent::drawNavAgentManager()
{
    if (m_navAgentManagerGroup == nullptr)
        return;
    m_navAgentManagerGroup->removeAllWidgets();

    auto navAgentManager = dynamic_cast<NavAgentManager*>(getComponent());
    if (navAgentManager == nullptr)
        return;

    std::array maxAgents = { (int)navAgentManager->getMaxAgentNumber() };
    m_navAgentManagerGroup->createWidget<Drag<int>>("Max Agents", ImGuiDataType_S32, maxAgents, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgentManager = dynamic_cast<NavAgentManager*>(getComponent());
        navAgentManager->setMaxAgentNumber(val[0]);
        });

    std::array maxRadius = { navAgentManager->getMaxAgentRadius() };
    m_navAgentManagerGroup->createWidget<Drag<float>>("Max Agent Radius", ImGuiDataType_Float, maxRadius, 0.001f, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgentManager = dynamic_cast<NavAgentManager*>(getComponent());
        navAgentManager->setMaxAgentRadius(val[0]);
        });
}
NS_IGE_END