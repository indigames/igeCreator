#include "core/scene/components/light/PointLightEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/light/PointLight.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

PointLightEditorComponent::PointLightEditorComponent() {
    m_pointLightGroup = nullptr;
}

PointLightEditorComponent::~PointLightEditorComponent() {
    m_pointLightGroup = nullptr;
}

void PointLightEditorComponent::onInspectorUpdate() {
    drawPointLight();
}

void PointLightEditorComponent::drawPointLight() {
    if (m_pointLightGroup == nullptr)
        m_pointLightGroup = m_group->createWidget<Group>("PointLight", false);;
    m_pointLightGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto color = Vec4(comp->getProperty<Vec3>("col", {}), 1.f);
    m_pointLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("col", { val[0], val[1], val[2] });
    });

    std::array its = { comp->getProperty<float>("its", 1.f) };
    m_pointLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, its)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("its", val[0]);
    });

    std::array rng = { comp->getProperty<float>("rng", 1.f) };
    m_pointLightGroup->createWidget<Drag<float>>("Range", ImGuiDataType_Float, rng)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("rng", val[0]);
    });
}
NS_IGE_END