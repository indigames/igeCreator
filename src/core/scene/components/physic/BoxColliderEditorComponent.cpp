#include "core/scene/components/physic/BoxColliderEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/collider/BoxCollider.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

BoxColliderEditorComponent::BoxColliderEditorComponent() {
    m_physicGroup = nullptr;
}

BoxColliderEditorComponent::~BoxColliderEditorComponent() {
    m_physicGroup = nullptr;
}

void BoxColliderEditorComponent::onInspectorUpdate() {
    drawBoxCollider();
}

void BoxColliderEditorComponent::drawBoxCollider() {
    if (m_physicGroup == nullptr)
        m_physicGroup = m_group->createWidget<Group>("ColliderGroup", false);;
    m_physicGroup->removeAllWidgets();

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

    std::array margin = { comp->getProperty<float>("margin", NAN) };
    auto m2 = m_physicGroup->createWidget<Drag<float>>("Margin", ImGuiDataType_Float, margin, 0.001f, 0.0f);
    m2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    m2->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("margin", val[0]);
    });
}
NS_IGE_END