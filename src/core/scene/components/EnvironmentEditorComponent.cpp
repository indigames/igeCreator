#include "core/scene/components/EnvironmentEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "components/EnvironmentComponent.h"

#include "core/layout/Columns.h"
#include "core/widgets/Widgets.h"
#include <core/layout/Group.h>
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

EnvironmentEditorComponent::EnvironmentEditorComponent() {
    m_environmentCompGroup = nullptr;
}

EnvironmentEditorComponent::~EnvironmentEditorComponent()
{
    m_environmentCompGroup = nullptr;
}

void EnvironmentEditorComponent::onInspectorUpdate() {
    drawEnvironmentComponent();
}

void EnvironmentEditorComponent::drawEnvironmentComponent() {
    if (m_environmentCompGroup == nullptr) {
        m_environmentCompGroup = m_group->createWidget<Group>("EnvironmentGroup", false);
    }
    m_environmentCompGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    // Shadow
    auto shadowGroup = m_environmentCompGroup->createWidget<Group>("Shadow");
    auto shadowColor = Vec4(comp->getProperty<Vec3>("shadowCol", {0.f, 0.f, 0.f}), 1.f);
    shadowGroup->createWidget<Color>("Color", shadowColor)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("shadowCol", Vec3(val[0], val[1], val[2]));
    });

    auto size = comp->getProperty<Vec2>("shadowSize", { 0.f, 0.f });
    std::array sizeArr = { size[0], size[1] };
    shadowGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, sizeArr, 1, 512, 4096)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("shadowSize", Vec2(val[0], val[1]));
    });

    std::array density = { comp->getProperty<float>("shadowDensity", 0.5f) };
    shadowGroup->createWidget<Drag<float>>("Density", ImGuiDataType_Float, density, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("shadowDensity", val[0]);
    });

    std::array wideness = { comp->getProperty<float>("shadowWideness", 1000.f) };
    shadowGroup->createWidget<Drag<float>>("Wideness", ImGuiDataType_Float, wideness, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("shadowWideness", val[0]);
    });

    std::array bias = { comp->getProperty<float>("shadowBias", 0.005f) };
    shadowGroup->createWidget<Drag<float>>("Bias", ImGuiDataType_Float, bias, 0.0001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("shadowBias", val[0]);
    });

    // Fog
    auto fogGroup = m_environmentCompGroup->createWidget<Group>("Fog");
    auto fogColor = Vec4(comp->getProperty<Vec3>("fogCol", {0.f, 0.f, 0.f}), comp->getProperty<float>("fogAlpha", 1.f));
    fogGroup->createWidget<Color>("Color", fogColor)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("fogCol", { val[0], val[1], val[2] });
        getComponent<CompoundComponent>()->setProperty("fogAlpha", val[3]);
    });

    auto fogColumn = fogGroup->createWidget<Columns<2>>(120.f);
    std::array fogNear = { comp->getProperty<float>("fogNear", 0.1f) };
    fogColumn->createWidget<Drag<float>>("Near", ImGuiDataType_Float, fogNear, 0.01f, 0.1f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("fogNear", val[0]);
    });

    std::array fogFar = { comp->getProperty<float>("fogFar", 50.f) };
    fogColumn->createWidget<Drag<float>>("Far", ImGuiDataType_Float, fogFar, 0.01f, 0.1f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("fogFar", val[0]);
    });
}

NS_IGE_END