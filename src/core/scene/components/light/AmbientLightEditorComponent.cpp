#include "core/scene/components/light/AmbientLightEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/light/AmbientLight.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

AmbientLightEditorComponent::AmbientLightEditorComponent() {
    m_ambientLightGroup = nullptr;
}

AmbientLightEditorComponent::~AmbientLightEditorComponent() {
    m_ambientLightGroup = nullptr;
}

void AmbientLightEditorComponent::onInspectorUpdate() {
    drawAmbientLight();
}

void AmbientLightEditorComponent::drawAmbientLight() {
    if (m_ambientLightGroup == nullptr)
        m_ambientLightGroup = m_group->createWidget<Group>("AmbientLight", false);;
    m_ambientLightGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto color = Vec4(comp->getProperty<Vec3>("sky", { NAN, NAN, NAN }), 1.f);
    m_ambientLightGroup->createWidget<Color>("SkyColor", color)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("sky", { val[0], val[1], val[2] });
    });

    color = Vec4(comp->getProperty<Vec3>("gnd", { NAN, NAN, NAN }), 1.f);
    m_ambientLightGroup->createWidget<Color>("GroundColor", color)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("gnd", { val[0], val[1], val[2] });
    });

    auto dir = comp->getProperty<Vec3>("dir", { NAN, NAN, NAN });
    std::array direction = { dir.X(), dir.Y(), dir.Z() };
    m_ambientLightGroup->createWidget<Drag<float, 3>>("Direction", ImGuiDataType_Float, direction)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("dir", { val[0], val[1], val[2] });
    });
}
NS_IGE_END