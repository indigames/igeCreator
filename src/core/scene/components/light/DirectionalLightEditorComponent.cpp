#include "core/scene/components/light/DirectionalLightEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/light/DirectionalLight.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

DirectionalLightEditorComponent::DirectionalLightEditorComponent() {
    m_directionalLightGroup = nullptr;
}

DirectionalLightEditorComponent::~DirectionalLightEditorComponent() {
    m_directionalLightGroup = nullptr;
}

void DirectionalLightEditorComponent::onInspectorUpdate() {
    drawDirectionalLight();
}

void DirectionalLightEditorComponent::drawDirectionalLight() {
    if (m_directionalLightGroup == nullptr)
        m_directionalLightGroup = m_group->createWidget<Group>("DirectionalLight", false);;
    m_directionalLightGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto color = Vec4(comp->getProperty<Vec3>("col", {}), 1.f);
    m_directionalLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("col", { val[0], val[1], val[2] });
    });

    std::array its = { comp->getProperty<float>("its", 1.f) };
    m_directionalLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, its)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("its", val[0]);
    });
}
NS_IGE_END