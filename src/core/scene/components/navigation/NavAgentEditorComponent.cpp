#include "core/scene/components/navigation/NavAgentEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavAgent.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavAgentEditorComponent::NavAgentEditorComponent() {
    m_navAgentGroup = nullptr;
}

NavAgentEditorComponent::~NavAgentEditorComponent() {
    m_navAgentGroup = nullptr;
}

void NavAgentEditorComponent::onInspectorUpdate() {    
    drawNavAgent();
}

void NavAgentEditorComponent::drawNavAgent()
{
    if (m_navAgentGroup == nullptr)
        m_navAgentGroup = m_group->createWidget<Group>("NavAgent", false);;
    m_navAgentGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_navAgentGroup->createWidget<CheckBox>("SyncPosition", comp->getProperty<bool>("syncPos", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("syncPos", val);
    });

    std::array radius = { comp->getProperty<float>("radius", NAN) };
    m_navAgentGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("radius", val[0]);
    });

    std::array height = { comp->getProperty<float>("height", NAN) };
    m_navAgentGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("height", val[0]);
    });

    std::array maxAcceleration = { comp->getProperty<float>("maxAcc", NAN) };
    m_navAgentGroup->createWidget<Drag<float>>("MaxAccel", ImGuiDataType_Float, maxAcceleration, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("maxAcc", val[0]);
    });

    std::array maxSpeed = { comp->getProperty<float>("maxSpeed", NAN) };
    m_navAgentGroup->createWidget<Drag<float>>("MaxSpeed", ImGuiDataType_Float, maxSpeed, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("maxSpeed", val[0]);
    });

    auto pos = comp->getProperty<Vec3>("targetPos", { NAN, NAN, NAN });
    std::array targetPosition = { pos.X(), pos.Y(), pos.Z() };
    m_navAgentGroup->createWidget<Drag<float, 3>>("TargetPos", ImGuiDataType_Float, targetPosition, 0.001f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("targetPos", { val[0], val[1], val[2] });
    });

    std::array queryFilterType = { comp->getProperty<float>("filter", NAN) };
    m_navAgentGroup->createWidget<Drag<float>>("FilterType", ImGuiDataType_S32, queryFilterType, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("filter", (int)val[0]);
    });

    std::array obstacleAvoidanceType = { comp->getProperty<float>("obsAvoid", NAN) };
    m_navAgentGroup->createWidget<Drag<float>>("ObsAvoidType", ImGuiDataType_S32, obstacleAvoidanceType, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("obsAvoid", (int)val[0]);
    });

    std::array navigationQuality = { comp->getProperty<float>("navQuality", NAN) };
    m_navAgentGroup->createWidget<Drag<float>>("NavQuality", ImGuiDataType_S32, navigationQuality, 1, (int)NavAgent::NavQuality::LOW, (int)NavAgent::NavQuality::HIGH)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("navQuality", (int)val[0]);
    });

    std::array navigationPushiness = { comp->getProperty<float>("navPushiness", NAN) };
    m_navAgentGroup->createWidget<Drag<float>>("NavPushiness", ImGuiDataType_S32, navigationPushiness, 1, (int)NavAgent::NavPushiness::LOW, (int)NavAgent::NavPushiness::HIGH)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("navPushiness", (int)val[0]);
    });
}
NS_IGE_END