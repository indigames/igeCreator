#include "core/scene/components/physic/SphereColliderEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/collider/SphereCollider.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

SphereColliderEditorComponent::SphereColliderEditorComponent() {
    m_physicGroup = nullptr;
}

SphereColliderEditorComponent::~SphereColliderEditorComponent()
{
    m_physicGroup = nullptr;
}

void SphereColliderEditorComponent::onInspectorUpdate() {
    drawSphereCollider();
}

void SphereColliderEditorComponent::drawSphereCollider()
{
    if (m_physicGroup == nullptr)
        m_physicGroup = m_group->createWidget<Group>("ColliderGroup", false);;
    m_physicGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    std::array radius = { comp->getProperty<float>("radius", NAN) };
    auto r = m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.0f);
    r->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    r->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("radius", val[0]);
    });
}
NS_IGE_END