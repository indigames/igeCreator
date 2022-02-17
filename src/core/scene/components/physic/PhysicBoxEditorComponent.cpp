#include "core/scene/components/physic/PhysicBoxEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicBox.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

PhysicBoxEditorComponent::PhysicBoxEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicBoxEditorComponent::~PhysicBoxEditorComponent() {
    m_physicGroup = nullptr;
}

void PhysicBoxEditorComponent::onInspectorUpdate() {
    drawPhysicBox();
}

void PhysicBoxEditorComponent::drawPhysicBox() {
    // Draw common properties
    drawPhysicObject();

    // Draw physic box properties
    m_physicGroup->createWidget<Separator>();
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto size = comp->getProperty<Vec3>("size", { NAN, NAN, NAN });
    std::array sizeArr = { size.X(), size.Y(), size.Z() };
    auto s1 = m_physicGroup->createWidget<Drag<float, 3>>("Size", ImGuiDataType_Float, sizeArr, 0.001f, 0.0f);
    s1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    s1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("size", { val[0], val[1], val[2] });
    });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END