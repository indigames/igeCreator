#include "core/scene/components/physic/PhysicCapsuleEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicCapsule.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

PhysicCapsuleEditorComponent::PhysicCapsuleEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicCapsuleEditorComponent::~PhysicCapsuleEditorComponent() {
    m_physicGroup = nullptr;
}

void PhysicCapsuleEditorComponent::onInspectorUpdate() {
    drawPhysicCapsule();
}

void PhysicCapsuleEditorComponent::drawPhysicCapsule() {
    // Draw common properties
    drawPhysicObject();

    // Draw physic box properties
    m_physicGroup->createWidget<Separator>();
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    std::array height = { comp->getProperty<float>("height", NAN) };
    m_physicGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("height", val[0]);
    });

    std::array radius = { comp->getProperty<float>("radius", NAN) };
    m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("radius", val[0]);
    });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END