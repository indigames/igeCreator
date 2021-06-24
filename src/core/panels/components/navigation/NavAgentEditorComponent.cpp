#include "core/panels/components/navigation/NavAgentEditorComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavAgent.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavAgentEditorComponent::NavAgentEditorComponent() {
    m_navAgentGroup = nullptr;
}

NavAgentEditorComponent::~NavAgentEditorComponent()
{
    if (m_navAgentGroup) {
        m_navAgentGroup->removeAllWidgets();
        m_navAgentGroup->removeAllPlugins();
    }
    m_navAgentGroup = nullptr;
}

bool NavAgentEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<NavAgent*>(comp);
}

void NavAgentEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_navAgentGroup == nullptr) {
        m_navAgentGroup = m_group->createWidget<Group>("NavAgent", false);
    }
    drawNavAgent();

    EditorComponent::redraw();
}

void NavAgentEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_navAgentGroup = m_group->createWidget<Group>("NavAgent", false);

    drawNavAgent();
}

void NavAgentEditorComponent::drawNavAgent()
{
    if (m_navAgentGroup == nullptr)
        return;
    m_navAgentGroup->removeAllWidgets();

    auto navAgent = dynamic_cast<NavAgent*>(getComponent());
    if (navAgent == nullptr)
        return;

    auto column = m_navAgentGroup->createWidget<Columns<3>>();
    column->createWidget<CheckBox>("Enable", navAgent->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setEnabled(val);
        });
    column->createWidget<CheckBox>("Sync Position", navAgent->isUpdateNodePosition())->getOnDataChangedEvent().addListener([this](bool val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setUpdateNodePosition(val);
        });

    std::array radius = { navAgent->getRadius() };
    m_navAgentGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setRadius(val[0]);
        });

    std::array height = { navAgent->getHeight() };
    m_navAgentGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setHeight(val[0]);
        });

    std::array maxAcceleration = { navAgent->getMaxAcceleration() };
    m_navAgentGroup->createWidget<Drag<float>>("Max Acceleration", ImGuiDataType_Float, maxAcceleration, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setMaxAcceleration(val[0]);
        });

    std::array maxSpeed = { navAgent->getMaxSpeed() };
    m_navAgentGroup->createWidget<Drag<float>>("Max Speed", ImGuiDataType_Float, maxSpeed, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setMaxSpeed(val[0]);
        });

    std::array targetPosition = { navAgent->getTargetPosition().X(), navAgent->getTargetPosition().Y(), navAgent->getTargetPosition().Z() };
    m_navAgentGroup->createWidget<Drag<float, 3>>("Target Position", ImGuiDataType_Float, targetPosition, 0.001f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setTargetPosition({ val[0], val[1], val[2] });
        });

    std::array queryFilterType = { (int)navAgent->getQueryFilterType() };
    m_navAgentGroup->createWidget<Drag<int>>("Query Filter Type", ImGuiDataType_S32, queryFilterType, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setQueryFilterType(val[0]);
        });

    std::array obstacleAvoidanceType = { (int)navAgent->getObstacleAvoidanceType() };
    m_navAgentGroup->createWidget<Drag<int>>("Obstacle Avoidance Type", ImGuiDataType_S32, obstacleAvoidanceType, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setObstacleAvoidanceType(val[0]);
        });

    std::array navigationQuality = { (int)navAgent->getNavigationQuality() };
    m_navAgentGroup->createWidget<Drag<int>>("Navigation Quality", ImGuiDataType_S32, navigationQuality, 1, (int)NavAgent::NavQuality::LOW, (int)NavAgent::NavQuality::HIGH)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setNavigationQuality((NavAgent::NavQuality)val[0]);
        });

    std::array navigationPushiness = { (int)navAgent->getNavigationPushiness() };
    m_navAgentGroup->createWidget<Drag<int>>("Navigation Pushiness", ImGuiDataType_S32, navigationPushiness, 1, (int)NavAgent::NavPushiness::LOW, (int)NavAgent::NavPushiness::HIGH)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navAgent = dynamic_cast<NavAgent*>(getComponent());
        navAgent->setNavigationPushiness((NavAgent::NavPushiness)val[0]);
        });
}
NS_IGE_END