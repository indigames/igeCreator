#include "core/panels/components/light/PointLightEditorComponent.h"

#include <core/layout/Group.h>

#include "components/light/PointLight.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

PointLightEditorComponent::PointLightEditorComponent() {
    m_pointLightGroup = nullptr;
}

PointLightEditorComponent::~PointLightEditorComponent()
{
    if (m_pointLightGroup) {
        m_pointLightGroup->removeAllWidgets();
        m_pointLightGroup->removeAllPlugins();
    }
    m_pointLightGroup = nullptr;
}

bool PointLightEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<PointLight*>(comp);
}

void PointLightEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_pointLightGroup == nullptr) {
        m_pointLightGroup = m_group->createWidget<Group>("PointLight", false);
    }
    drawPointLight();

    EditorComponent::redraw();
}

void PointLightEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_pointLightGroup = m_group->createWidget<Group>("PointLight", false);

    drawPointLight();
}

void PointLightEditorComponent::drawPointLight()
{
    if (m_pointLightGroup == nullptr)
        return;
    m_pointLightGroup->removeAllWidgets();

    auto pointLight = m_targetObject->getComponent<PointLight>();
    if (pointLight == nullptr)
        return;

    auto color = Vec4(pointLight->getColor(), 1.f);
    m_pointLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto pointLight = m_targetObject->getComponent<PointLight>();
        pointLight->setColor({ val[0], val[1], val[2] });
        });

    std::array intensity = { pointLight->getIntensity() };
    m_pointLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, intensity, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto pointLight = m_targetObject->getComponent<PointLight>();
        pointLight->setIntensity(val[0]);
        });

    std::array range = { pointLight->getRange() };
    m_pointLightGroup->createWidget<Drag<float>>("Range", ImGuiDataType_Float, range, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto pointLight = m_targetObject->getComponent<PointLight>();
        pointLight->setRange(val[0]);
        });
}
NS_IGE_END