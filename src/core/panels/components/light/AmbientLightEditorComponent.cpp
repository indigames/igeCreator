#include "core/panels/components/light/AmbientLightEditorComponent.h"

#include <core/layout/Group.h>

#include "components/light/AmbientLight.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

AmbientLightEditorComponent::AmbientLightEditorComponent() {
    m_ambientLightGroup = nullptr;
}

AmbientLightEditorComponent::~AmbientLightEditorComponent()
{
    if (m_ambientLightGroup) {
        m_ambientLightGroup->removeAllWidgets();
        m_ambientLightGroup->removeAllPlugins();
    }
    m_ambientLightGroup = nullptr;
}

bool AmbientLightEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<AmbientLight*>(comp);
}

void AmbientLightEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_ambientLightGroup == nullptr) {
        m_ambientLightGroup = m_group->createWidget<Group>("AmbientLight", false);
    }
    drawAmbientLight();

    EditorComponent::redraw();
}

void AmbientLightEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_ambientLightGroup = m_group->createWidget<Group>("AmbientLight", false);

    drawAmbientLight();
}

void AmbientLightEditorComponent::drawAmbientLight()
{
    if (m_ambientLightGroup == nullptr)
        return;
    m_ambientLightGroup->removeAllWidgets();

    auto ambientLight = m_targetObject->getComponent<AmbientLight>();
    if (ambientLight == nullptr)
        return;

    auto color = Vec4(ambientLight->getSkyColor(), 1.f);
    m_ambientLightGroup->createWidget<Color>("SkyColor", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto ambientLight = m_targetObject->getComponent<AmbientLight>();
        ambientLight->setSkyColor({ val[0], val[1], val[2] });
        });

    color = Vec4(ambientLight->getGroundColor(), 1.f);
    m_ambientLightGroup->createWidget<Color>("GroundColor", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto ambientLight = m_targetObject->getComponent<AmbientLight>();
        ambientLight->setGroundColor({ val[0], val[1], val[2] });
        });

    std::array direction = { ambientLight->getDirection().X(), ambientLight->getDirection().Y(), ambientLight->getDirection().Z() };
    m_ambientLightGroup->createWidget<Drag<float, 3>>("Direction", ImGuiDataType_Float, direction)->getOnDataChangedEvent().addListener([this](auto val) {
        auto ambientLight = m_targetObject->getComponent<AmbientLight>();
        ambientLight->setDirection({ val[0], val[1], val[2] });
        });
}
NS_IGE_END