#include "core/panels/components/gui/CanvasEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/Canvas.h"

#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

CanvasEditorComponent::CanvasEditorComponent() {
    m_canvasGroup = nullptr;
}

CanvasEditorComponent::~CanvasEditorComponent()
{
    if (m_canvasGroup) {
        m_canvasGroup->removeAllWidgets();
        m_canvasGroup->removeAllPlugins();
    }
    m_canvasGroup = nullptr;
}

bool CanvasEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<Canvas*>(comp);
}

void CanvasEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_canvasGroup == nullptr) {
        m_canvasGroup = m_group->createWidget<Group>("CanvasGroup", false);
    }
    drawCanvas();

    EditorComponent::redraw();
}

void CanvasEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_canvasGroup = m_group->createWidget<Group>("CanvasGroup", false);

    drawCanvas();
}

void CanvasEditorComponent::drawCanvas()
{
    if (m_canvasGroup == nullptr)
        return;
    m_canvasGroup->removeAllWidgets();

    auto canvas = m_targetObject->getCanvas();
    if (canvas == nullptr)
        return;

    std::array size = { canvas->getDesignCanvasSize().X(), canvas->getDesignCanvasSize().Y() };
    m_canvasGroup->createWidget<Drag<float, 2>>("Design Size", ImGuiDataType_Float, size)->getOnDataChangedEvent().addListener([this](auto val) {
        auto canvas = m_targetObject->getCanvas();
        canvas->setDesignCanvasSize({ val[0], val[1] });
        auto transformToViewport = Mat4::Translate(Vec3(-val[0] * 0.5f, -val[1] * 0.5f, 0.f));
        canvas->setCanvasToViewportMatrix(transformToViewport);
        });

    std::array targetSize = { canvas->getTargetCanvasSize().X(), canvas->getTargetCanvasSize().Y() };
    m_canvasGroup->createWidget<Drag<float, 2>>("Target Size", ImGuiDataType_Float, targetSize)->getOnDataChangedEvent().addListener([this](auto val) {
        auto canvas = m_targetObject->getCanvas();
        canvas->setTargetCanvasSize({ val[0], val[1] });
        });
}
NS_IGE_END