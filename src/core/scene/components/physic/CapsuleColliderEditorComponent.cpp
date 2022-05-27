#include "core/scene/components/physic/CapsuleColliderEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/CapsuleCollider.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

CapsuleColliderEditorComponent::CapsuleColliderEditorComponent() {
    m_physicGroup = nullptr;
}

CapsuleColliderEditorComponent::~CapsuleColliderEditorComponent() {
    m_physicGroup = nullptr;
}

void CapsuleColliderEditorComponent::onInspectorUpdate() {
    drawCapsuleCollider();
}

void CapsuleColliderEditorComponent::drawCapsuleCollider() {
    if (m_physicGroup == nullptr)
        m_physicGroup = m_group->createWidget<Group>("ColliderGroup", false);;
    m_physicGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    std::array height = { comp->getProperty<float>("height", NAN) };
    auto h1 = m_physicGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.0f);
    h1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    h1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("height", val[0]);
    });

    std::array radius = { comp->getProperty<float>("radius", NAN) };
    auto r1 = m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.0f);
    r1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    r1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("radius", val[0]);
    });
}
NS_IGE_END