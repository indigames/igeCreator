#include "core/scene/components/physic/PhysicSphereEditorComponent.h"
#include "core/scene/CompoundComponent.h"

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
    m_physicGroup = nullptr;
}

void PhysicSphereEditorComponent::onInspectorUpdate() {
    drawPhysicSphere();
}

void PhysicSphereEditorComponent::drawPhysicSphere()
{
    drawPhysicObject();

    // Draw properties
    m_physicGroup->createWidget<Separator>();
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    std::array radius = { comp->getProperty<float>("radius", 1.f) };
    m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("radius", val[0]);
    });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END