#include "core/panels/components/physic/PhysicSphereEditorComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicSphere.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

PhysicSphereEditorComponent::PhysicSphereEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicSphereEditorComponent::~PhysicSphereEditorComponent()
{
    if (m_physicGroup) {
        m_physicGroup->removeAllWidgets();
        m_physicGroup->removeAllPlugins();
    }
    m_physicGroup = nullptr;
}

bool PhysicSphereEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<PhysicSphere*>(comp);
}

void PhysicSphereEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_physicGroup == nullptr) {
        m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);
    }
    drawPhysicSphere();

    EditorComponent::redraw();
}

void PhysicSphereEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);

    drawPhysicSphere();
}

void PhysicSphereEditorComponent::drawPhysicSphere()
{
    auto physicComp = m_targetObject->getComponent<PhysicSphere>();
    if (physicComp == nullptr)
        return;

    drawPhysicObject();

    m_physicGroup->createWidget<Separator>();
    std::array radius = { physicComp->getRadius() };
    m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = m_targetObject->getComponent<PhysicSphere>();
        physicComp->setRadius(val[0]);
        });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END