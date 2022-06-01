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
        m_physicManagerGroup = m_group->createWidget<Group>("PhysicManagerGroup", false);
    m_physicManagerGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    if (getComponent<CompoundComponent>()->size() <= 0)
        return;

    auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
    if (physicComp == nullptr)
        return;

    auto columns = m_physicManagerGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("Deformable", physicComp->isDeformable())->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setDeformable(val);
    });
    columns->createWidget<CheckBox>("Debug", physicComp->isShowDebug())->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setShowDebug(val);
    });

    std::array numItr = { (float)physicComp->getNumIteration() };
    auto i1 = m_physicManagerGroup->createWidget<Drag<float>>("NumIterations", ImGuiDataType_S32, numItr, 1, 1, 32);
    i1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    i1->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setNumIteration((int)val[0]);
    });

    std::array subSteps = { (float)physicComp->getFrameMaxSubStep() };
    auto m1 = m_physicManagerGroup->createWidget<Drag<float>>("NumSubsteps", ImGuiDataType_S32, subSteps, 1, 1, 32);
    m1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    m1->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setFrameMaxSubStep((int)val[0]);
    });

    std::array timeStep = { physicComp->getFixedTimeStep() };
    auto t1 = m_physicManagerGroup->createWidget<Drag<float>>("TimeStep", ImGuiDataType_Float, timeStep, 0.001f, 0.001f);
    t1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    t1->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setFixedTimeStep(val[0]);
    });

    std::array frameRatio = { physicComp->getFrameUpdateRatio() };
    auto u1 = m_physicManagerGroup->createWidget<Drag<float>>("UpdateRatio", ImGuiDataType_Float, frameRatio, 0.001f, 0.001f);
    u1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    u1->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setFrameUpdateRatio(val[0]);
    });

    std::array gravity = { physicComp->getGravity().x(), physicComp->getGravity().y(), physicComp->getGravity().z() };
    auto g1 = m_physicManagerGroup->createWidget<Drag<float, 3>>("Gravity", ImGuiDataType_Float, gravity, 0.001f);
    g1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    g1->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = std::dynamic_pointer_cast<PhysicManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        physicComp->setGravity({ val[0], val[1], val[2] });
    });
}
NS_IGE_END