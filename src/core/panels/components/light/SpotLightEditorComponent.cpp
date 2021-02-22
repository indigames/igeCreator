#include "core/panels/components/light/SpotLightEditorComponent.h"

#include <core/layout/Group.h>

#include "components/light/SpotLight.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

SpotLightEditorComponent::SpotLightEditorComponent() {
    m_spotLightGroup = nullptr;
}

SpotLightEditorComponent::~SpotLightEditorComponent()
{
    if (m_spotLightGroup) {
        m_spotLightGroup->removeAllWidgets();
        m_spotLightGroup->removeAllPlugins();
    }
    m_spotLightGroup = nullptr;
}

bool SpotLightEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<SpotLight*>(comp);
}

void SpotLightEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_spotLightGroup == nullptr) {
        m_spotLightGroup = m_group->createWidget<Group>("SpotLight", false);
    }
    drawSpotLight();

    EditorComponent::redraw();
}

void SpotLightEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_spotLightGroup = m_group->createWidget<Group>("SpotLight", false);

    drawSpotLight();
}

void SpotLightEditorComponent::drawSpotLight()
{
    if (m_spotLightGroup == nullptr)
        return;
    m_spotLightGroup->removeAllWidgets();

    auto light = m_targetObject->getComponent<SpotLight>();
    if (light == nullptr)
        return;

    auto color = Vec4(light->getColor(), 1.f);
    m_spotLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto light = m_targetObject->getComponent<SpotLight>();
        light->setColor({ val[0], val[1], val[2] });
        });

    std::array intensity = { light->getIntensity() };
    m_spotLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, intensity, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto light = m_targetObject->getComponent<SpotLight>();
        light->setIntensity(val[0]);
        });

    std::array range = { light->getRange() };
    m_spotLightGroup->createWidget<Drag<float>>("Range", ImGuiDataType_Float, range, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto light = m_targetObject->getComponent<SpotLight>();
        light->setRange(val[0]);
        });

    std::array angle = { light->getAngle() };
    m_spotLightGroup->createWidget<Drag<float>>("Angle", ImGuiDataType_Float, angle, 0.01f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto light = m_targetObject->getComponent<SpotLight>();
        light->setAngle(val[0]);
        });
}
NS_IGE_END