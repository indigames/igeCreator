#include "core/scene/components/physic/PhysicManagerEditorComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicManager.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

PhysicManagerEditorComponent::PhysicManagerEditorComponent() {
    m_physicManagerGroup = nullptr;
}

PhysicManagerEditorComponent::~PhysicManagerEditorComponent()
{
    m_physicManagerGroup = nullptr;
}

void PhysicManagerEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_physicManagerGroup == nullptr) {
        m_physicManagerGroup = m_group->createWidget<Group>("PhysicManagerGroup", false);
    }
    drawPhysicManager();

    EditorComponent::redraw();
}

void PhysicManagerEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_physicManagerGroup = m_group->createWidget<Group>("PhysicManagerGroup", false);

    drawPhysicManager();
}

void PhysicManagerEditorComponent::drawPhysicManager()
{
    if (m_physicManagerGroup == nullptr)
        return;
    m_physicManagerGroup->removeAllWidgets();

    auto physicComp = getComponent<PhysicManager>();
    if (physicComp == nullptr)
        return;

    auto columns = m_physicManagerGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("Deformable", physicComp->isDeformable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = getComponent<PhysicManager>();
        physicComp->setDeformable(val);
        });
    columns->createWidget<CheckBox>("Debug", physicComp->isShowDebug())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = getComponent<PhysicManager>();
        physicComp->setShowDebug(val);
        });

    std::array numItr = { physicComp->getNumIteration() };
    m_physicManagerGroup->createWidget<Drag<int>>("Iterations Number", ImGuiDataType_S32, numItr, 1, 1, 32)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicManager>();
        physicComp->setNumIteration(val[0]);
        });

    std::array subSteps = { physicComp->getFrameMaxSubStep() };
    m_physicManagerGroup->createWidget<Drag<int>>("Max Substeps Number", ImGuiDataType_S32, subSteps, 1, 1, 32)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicManager>();
        physicComp->setFrameMaxSubStep(val[0]);
        });

    std::array timeStep = { physicComp->getFixedTimeStep() };
    m_physicManagerGroup->createWidget<Drag<float>>("Time Step", ImGuiDataType_Float, timeStep, 0.001f, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicManager>();
        physicComp->setFixedTimeStep(val[0]);
        });

    std::array frameRatio = { physicComp->getFrameUpdateRatio() };
    m_physicManagerGroup->createWidget<Drag<float>>("Update Ratio", ImGuiDataType_Float, frameRatio, 0.001f, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicManager>();
        physicComp->setFrameUpdateRatio(val[0]);
        });

    std::array gravity = { physicComp->getGravity().x(), physicComp->getGravity().y(), physicComp->getGravity().z() };
    m_physicManagerGroup->createWidget<Drag<float, 3>>("Gravity", ImGuiDataType_Float, gravity, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = getComponent<PhysicManager>();
        physicComp->setGravity({ val[0], val[1], val[2] });
        });
}
NS_IGE_END