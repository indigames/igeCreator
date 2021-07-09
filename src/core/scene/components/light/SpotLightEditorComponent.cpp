#include "core/scene/components/light/SpotLightEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/light/SpotLight.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

SpotLightEditorComponent::SpotLightEditorComponent() {
    m_spotLightGroup = nullptr;
}

SpotLightEditorComponent::~SpotLightEditorComponent() {
    m_spotLightGroup = nullptr;
}

void SpotLightEditorComponent::onInspectorUpdate() {
    drawSpotLight();
}

void SpotLightEditorComponent::drawSpotLight()
{
    if (m_spotLightGroup == nullptr)
        m_spotLightGroup = m_group->createWidget<Group>("SpotLight", false);
    m_spotLightGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto color = Vec4(comp->getProperty<Vec3>("col", {}), 1.f);
    m_spotLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("col", { val[0], val[1], val[2] });
    });

    std::array its = { comp->getProperty<float>("its", 1.f) };
    m_spotLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, its)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("its", val[0]);
    });

    std::array rng = { comp->getProperty<float>("rng", 1.f) };
    m_spotLightGroup->createWidget<Drag<float>>("Range", ImGuiDataType_Float, rng)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("rng", val[0]);
    });

    std::array angle = { comp->getProperty<float>("angle", 30.f) };
    m_spotLightGroup->createWidget<Drag<float>>("Range", ImGuiDataType_Float, angle)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("angle", val[0]);
    });
}
NS_IGE_END