#include "core/scene/components/physic/PhysicManagerEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicManager.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

PhysicManagerEditorComponent::PhysicManagerEditorComponent() {
    m_physicManagerGroup = nullptr;
}

PhysicManagerEditorComponent::~PhysicManagerEditorComponent() {
    m_physicManagerGroup = nullptr;
}

void PhysicManagerEditorComponent::onInspectorUpdate() {   
    drawPhysicManager();
}

void PhysicManagerEditorComponent::drawPhysicManager()
{
    if (m_physicManagerGroup == nullptr)
        m_physicManagerGroup = m_group->createWidget<Group>("PhysicManagerGroup", false);;
    m_physicManagerGroup->removeAllWidgets();

    if (getComponent<CompoundComponent>()->size() <= 0)
        return;

    auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
    if (physicComp == nullptr)
        return;

    auto columns = m_physicManagerGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("Deformable", physicComp->isDeformable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setDeformable(val);
    });
    columns->createWidget<CheckBox>("Debug", physicComp->isShowDebug())->getOnDataChangedEvent().addListener([this](bool val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setShowDebug(val);
    });

    std::array numItr = { physicComp->getNumIteration() };
    m_physicManagerGroup->createWidget<Drag<int>>("Iterations Number", ImGuiDataType_S32, numItr, 1, 1, 32)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setNumIteration(val[0]);
    });

    std::array subSteps = { physicComp->getFrameMaxSubStep() };
    m_physicManagerGroup->createWidget<Drag<int>>("Max Substeps Number", ImGuiDataType_S32, subSteps, 1, 1, 32)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setFrameMaxSubStep(val[0]);
    });

    std::array timeStep = { physicComp->getFixedTimeStep() };
    m_physicManagerGroup->createWidget<Drag<float>>("Time Step", ImGuiDataType_Float, timeStep, 0.001f, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setFixedTimeStep(val[0]);
    });

    std::array frameRatio = { physicComp->getFrameUpdateRatio() };
    m_physicManagerGroup->createWidget<Drag<float>>("Update Ratio", ImGuiDataType_Float, frameRatio, 0.001f, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setFrameUpdateRatio(val[0]);
    });

    std::array gravity = { physicComp->getGravity().x(), physicComp->getGravity().y(), physicComp->getGravity().z() };
    m_physicManagerGroup->createWidget<Drag<float, 3>>("Gravity", ImGuiDataType_Float, gravity, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setGravity({ val[0], val[1], val[2] });
    });
}
NS_IGE_END