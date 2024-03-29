#include "core/scene/components/navigation/NavAgentManagerEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavAgentManager.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavAgentManagerEditorComponent::NavAgentManagerEditorComponent() {
    m_navAgentManagerGroup = nullptr;
}

NavAgentManagerEditorComponent::~NavAgentManagerEditorComponent() {
    m_navAgentManagerGroup = nullptr;
}

void NavAgentManagerEditorComponent::onInspectorUpdate() {
    drawNavAgentManager();
}

void NavAgentManagerEditorComponent::drawNavAgentManager()
{
    if (m_navAgentManagerGroup == nullptr)
        m_navAgentManagerGroup = m_group->createWidget<Group>("NavAgentManager", false);;
    m_navAgentManagerGroup->removeAllWidgets();

    if (getComponent<CompoundComponent>() && getComponent<CompoundComponent>()->size() == 1)
    {
        auto navAgentManager = std::dynamic_pointer_cast<NavAgentManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        if (navAgentManager == nullptr)
            return;

        std::array maxAgents = { (float)navAgentManager->getMaxAgentNumber() };
        auto m1 = m_navAgentManagerGroup->createWidget<Drag<float>>("Max Agents", ImGuiDataType_S32, maxAgents, 1, 0);
        m1->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        m1->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgentManager = std::dynamic_pointer_cast<NavAgentManager>(getComponent<CompoundComponent>()->getComponents()[0]);
            navAgentManager->setMaxAgentNumber((int)val[0]);
        });

        std::array maxRadius = { navAgentManager->getMaxAgentRadius() };
        auto m2 = m_navAgentManagerGroup->createWidget<Drag<float>>("Max Agent Radius", ImGuiDataType_Float, maxRadius, 0.001f, 0);
        m2->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        m2->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgentManager = std::dynamic_pointer_cast<NavAgentManager>(getComponent<CompoundComponent>()->getComponents()[0]);
            navAgentManager->setMaxAgentRadius(val[0]);
        });
    }    
}
NS_IGE_END
