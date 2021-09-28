#include "core/scene/components/gui/CanvasEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/gui/Canvas.h"

#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

CanvasEditorComponent::CanvasEditorComponent() {
    m_canvasGroup = nullptr;
}

CanvasEditorComponent::~CanvasEditorComponent() {
    m_canvasGroup = nullptr;
}

void CanvasEditorComponent::onInspectorUpdate() {
    drawCanvas();
}

void CanvasEditorComponent::drawCanvas()
{
    if (m_canvasGroup == nullptr)
        m_canvasGroup = m_group->createWidget<Group>("CanvasGroup", false);;
    m_canvasGroup->removeAllWidgets();

    auto canvas = std::dynamic_pointer_cast<Canvas>(getComponent<CompoundComponent>()->getComponents()[0]); 
    if (canvas == nullptr)
        return;

    std::array size = { canvas->getDesignCanvasSize().X(), canvas->getDesignCanvasSize().Y() };
    m_canvasGroup->createWidget<Drag<float, 2>>("DesignSize", ImGuiDataType_Float, size)->getOnDataChangedEvent().addListener([this](auto val) {
        auto canvas = std::dynamic_pointer_cast<Canvas>(getComponent<CompoundComponent>()->getComponents()[0]);
        canvas->setDesignCanvasSize({ val[0], val[1] });
    });

    std::array targetSize = { canvas->getTargetCanvasSize().X(), canvas->getTargetCanvasSize().Y() };
    m_canvasGroup->createWidget<Drag<float, 2>>("TargetSize", ImGuiDataType_Float, targetSize)->getOnDataChangedEvent().addListener([this](auto val) {
        auto canvas = std::dynamic_pointer_cast<Canvas>(getComponent<CompoundComponent>()->getComponents()[0]);
        canvas->setTargetCanvasSize({ val[0], val[1] });
    });
}
NS_IGE_END