#include "core/panels/components/physic/PhysicBoxEditorComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicBox.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

PhysicBoxEditorComponent::PhysicBoxEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicBoxEditorComponent::~PhysicBoxEditorComponent()
{
    if (m_physicGroup) {
        m_physicGroup->removeAllWidgets();
        m_physicGroup->removeAllPlugins();
    }
    m_physicGroup = nullptr;
}

bool PhysicBoxEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<PhysicBox*>(comp);
}

void PhysicBoxEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_physicGroup == nullptr) {
        m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);
    }
    drawPhysicBox();

    EditorComponent::redraw();
}

void PhysicBoxEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);

    drawPhysicBox();
}

void PhysicBoxEditorComponent::drawPhysicBox()
{
    auto physicComp = m_targetObject->getComponent<PhysicBox>();
    if (physicComp == nullptr)
        return;

    drawPhysicObject();

    m_physicGroup->createWidget<Separator>();
    std::array size = { physicComp->getSize().X(), physicComp->getSize().Y(), physicComp->getSize().Z() };
    m_physicGroup->createWidget<Drag<float, 3>>("Size", ImGuiDataType_Float, size, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = m_targetObject->getComponent<PhysicBox>();
        physicComp->setSize({ val[0], val[1], val[2] });
        });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END