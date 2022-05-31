#include "core/scene/components/physic/CompoundColliderEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/collider/CompoundCollider.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

CompoundColliderEditorComponent::CompoundColliderEditorComponent() {
    m_physicGroup = nullptr;
}

CompoundColliderEditorComponent::~CompoundColliderEditorComponent() {
    m_physicGroup = nullptr;
}

void CompoundColliderEditorComponent::onInspectorUpdate() {
    drawCollider();
}

void CompoundColliderEditorComponent::drawCollider() {
    if (m_physicGroup == nullptr)
        m_physicGroup = m_group->createWidget<Group>("ColliderGroup", false);;
    m_physicGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    // TODO: inspect properties
}
NS_IGE_END