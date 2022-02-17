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

    auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
    if (environment == nullptr)
        return;

    // Shadow
    auto shadowGroup = m_environmentCompGroup->createWidget<Group>("Shadow");
    auto shadowColor = Vec4(environment->getShadowColor(), 1.f);
    auto shadowColorE = shadowGroup->createWidget<Color>("Color", shadowColor);
    shadowColorE->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
        environment->setShadowColor({ val[0], val[1], val[2] });
    });

    std::array size = { environment->getShadowTextureSize()[0], environment->getShadowTextureSize()[1] };
    auto sizeE = shadowGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, size, 1, 512, 4096);
    sizeE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    sizeE->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
        environment->setShadowTextureSize(Vec2(val[0], val[1]));
    });

    std::array density = { environment->getShadowDensity() };
    auto densityE = shadowGroup->createWidget<Drag<float>>("Density", ImGuiDataType_Float, density, 0.01f, 0.f, 1.f);
    densityE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    densityE->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
        environment->setShadowDensity(val[0]);
    });

    std::array wideness = { environment->getShadowWideness() };
    auto widenessE = shadowGroup->createWidget<Drag<float>>("Wideness", ImGuiDataType_Float, wideness, 0.01f, 0.f);
    widenessE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    widenessE->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
        environment->setShadowWideness(val[0]);
    });

    std::array bias = { environment->getShadowBias() };
    auto biasE = shadowGroup->createWidget<Drag<float>>("Bias", ImGuiDataType_Float, bias, 0.0001f, 0.f);
    biasE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    biasE->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
        environment->setShadowBias(val[0]);
    });

    // Fog
    auto fogGroup = m_environmentCompGroup->createWidget<Group>("Fog");
    auto fogColor = Vec4(environment->getDistanceFogColor(), environment->getDistanceFogAlpha());
    fogGroup->createWidget<Color>("Color", fogColor)->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
        environment->setDistanceFogColor({ val[0], val[1], val[2] });
        environment->setDistanceFogAlpha(val[3]);
    });

    auto fogColumn = fogGroup->createWidget<Columns<2>>(120.f);
    std::array fogNear = { environment->getDistanceFogNear() };
    auto nearE = fogColumn->createWidget<Drag<float>>("Near", ImGuiDataType_Float, fogNear, 0.01f, 0.1f);
    nearE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    nearE->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
        environment->setDistanceFogNear(val[0]);
    });

    std::array fogFar = { environment->getDistanceFogFar() };
    auto farE = fogColumn->createWidget<Drag<float>>("Far", ImGuiDataType_Float, fogFar, 0.01f, 0.1f);
    farE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    farE->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = std::dynamic_pointer_cast<EnvironmentComponent>(getComponent<CompoundComponent>()->getComponents()[0]);
        environment->setDistanceFogFar(val[0]);
    });
}

NS_IGE_END