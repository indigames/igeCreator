#include "core/panels/components/light/DirectionalLightEditorComponent.h"

#include <core/layout/Group.h>

#include "components/light/DirectionalLight.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

DirectionalLightEditorComponent::DirectionalLightEditorComponent() {
    m_directionalLightGroup = nullptr;
}

DirectionalLightEditorComponent::~DirectionalLightEditorComponent()
{
    if (m_directionalLightGroup) {
        m_directionalLightGroup->removeAllWidgets();
        m_directionalLightGroup->removeAllPlugins();
    }
    m_directionalLightGroup = nullptr;
}

bool DirectionalLightEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<DirectionalLight*>(comp);
}

void DirectionalLightEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_directionalLightGroup == nullptr) {
        m_directionalLightGroup = m_group->createWidget<Group>("DirectionalLight", false);
    }
    drawDirectionalLight();

    EditorComponent::redraw();
}

void DirectionalLightEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_directionalLightGroup = m_group->createWidget<Group>("DirectionalLight", false);

    drawDirectionalLight();
}

void DirectionalLightEditorComponent::drawDirectionalLight()
{
    if (m_directionalLightGroup == nullptr)
        return;
    m_directionalLightGroup->removeAllWidgets();

    auto directionalLight = dynamic_cast<DirectionalLight*>(getComponent());
    if (directionalLight == nullptr)
        return;

    auto color = Vec4(directionalLight->getColor(), 1.f);
    m_directionalLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto directionalLight = dynamic_cast<DirectionalLight*>(getComponent());
        directionalLight->setColor({ val[0], val[1], val[2] });
        });

    std::array intensity = { directionalLight->getIntensity() };
    m_directionalLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, intensity, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto directionalLight = dynamic_cast<DirectionalLight*>(getComponent());
        directionalLight->setIntensity(val[0]);
        });
}
NS_IGE_END