#include "core/panels/components/EnvironmentEditorComponent.h"

#include <core/layout/Group.h>

#include "components/EnvironmentComponent.h"

#include "core/layout/Columns.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

EnvironmentEditorComponent::EnvironmentEditorComponent() {
    m_environmentCompGroup = nullptr;
}

EnvironmentEditorComponent::~EnvironmentEditorComponent()
{
    if (m_environmentCompGroup) {
        m_environmentCompGroup->removeAllWidgets();
        m_environmentCompGroup->removeAllPlugins();
    }
    m_environmentCompGroup = nullptr;
}

bool EnvironmentEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<EnvironmentComponent*>(comp);
}

void EnvironmentEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_environmentCompGroup == nullptr) {
        m_environmentCompGroup = m_group->createWidget<Group>("EnvironmentGroup", false);
    }
    drawEnvironmentComponent();

    EditorComponent::redraw();
}

void EnvironmentEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_environmentCompGroup = m_group->createWidget<Group>("EnvironmentGroup", false);

    drawEnvironmentComponent();
}

void EnvironmentEditorComponent::drawEnvironmentComponent()
{
    if (m_environmentCompGroup == nullptr)
        return;
    m_environmentCompGroup->removeAllWidgets();
    auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
    if (environment == nullptr)
        return;

    // Shadow
    auto shadowGroup = m_environmentCompGroup->createWidget<Group>("Shadow");
    auto shadowColor = Vec4(environment->getShadowColor(), 1.f);
    shadowGroup->createWidget<Color>("Color", shadowColor)->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
        environment->setShadowColor({ val[0], val[1], val[2] });
        });

    std::array size = { environment->getShadowTextureSize()[0], environment->getShadowTextureSize()[1] };
    shadowGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, size, 1, 512, 4096)->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
        environment->setShadowTextureSize(Vec2(val[0], val[1]));
        });

    std::array density = { environment->getShadowDensity() };
    shadowGroup->createWidget<Drag<float>>("Density", ImGuiDataType_Float, density, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
        environment->setShadowDensity(val[0]);
        });
    std::array wideness = { environment->getShadowWideness() };
    shadowGroup->createWidget<Drag<float>>("Wideness", ImGuiDataType_Float, wideness, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
        environment->setShadowWideness(val[0]);
        });
    std::array bias = { environment->getShadowBias() };
    shadowGroup->createWidget<Drag<float>>("Bias", ImGuiDataType_Float, bias, 0.0001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
        environment->setShadowBias(val[0]);
        });

    // Fog
    auto fogGroup = m_environmentCompGroup->createWidget<Group>("Fog");
    auto fogColor = Vec4(environment->getDistanceFogColor(), environment->getDistanceFogAlpha());
    fogGroup->createWidget<Color>("Color", fogColor)->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
        environment->setDistanceFogColor({ val[0], val[1], val[2] });
        environment->setDistanceFogAlpha(val[3]);
        });

    auto fogColumn = fogGroup->createWidget<Columns<2>>(120.f);
    std::array fogNear = { environment->getDistanceFogNear() };
    fogColumn->createWidget<Drag<float>>("Near", ImGuiDataType_Float, fogNear, 0.01f, 0.1f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
        environment->setDistanceFogNear(val[0]);
        });

    std::array fogFar = { environment->getDistanceFogFar() };
    fogColumn->createWidget<Drag<float>>("Far", ImGuiDataType_Float, fogFar, 0.01f, 0.1f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto environment = dynamic_cast<EnvironmentComponent*>(getComponent());
        environment->setDistanceFogFar(val[0]);
        });
}

NS_IGE_END