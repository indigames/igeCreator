#include "core/panels/components/physic/PhysicCapsuleEditorComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicCapsule.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

PhysicCapsuleEditorComponent::PhysicCapsuleEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicCapsuleEditorComponent::~PhysicCapsuleEditorComponent()
{
    if (m_physicGroup) {
        m_physicGroup->removeAllWidgets();
        m_physicGroup->removeAllPlugins();
    }
    m_physicGroup = nullptr;
}

bool PhysicCapsuleEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<PhysicCapsule*>(comp);
}

void PhysicCapsuleEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_physicGroup == nullptr) {
        m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);
    }
    drawPhysicCapsule();

    EditorComponent::redraw();
}

void PhysicCapsuleEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);

    drawPhysicCapsule();
}

void PhysicCapsuleEditorComponent::drawPhysicCapsule()
{
    auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
    if (physicComp == nullptr)
        return;

    drawPhysicObject();

    m_physicGroup->createWidget<Separator>();
    std::array height = { physicComp->getHeight() };
    m_physicGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
        physicComp->setHeight(val[0]);
        });
    std::array radius = { physicComp->getRadius() };
    m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
        physicComp->setRadius(val[0]);
        });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END