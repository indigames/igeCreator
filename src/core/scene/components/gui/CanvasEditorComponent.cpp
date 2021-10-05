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

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

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

    auto mode = canvas->getScreenMatchMode();
    auto modeCombo = m_canvasGroup->createWidget<ComboBox>("Screen Match Mode", (int)mode);
    modeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            auto canvas = std::dynamic_pointer_cast<Canvas>(getComponent<CompoundComponent>()->getComponents()[0]);
            canvas->setScreenMatchMode((int)val);
            setDirty();
        }
        });
    modeCombo->setEndOfLine(false);
    modeCombo->addChoice((int)ScreenMatchMode::MatchWidthOrHeight, "Match Width Or Height");
    modeCombo->addChoice((int)ScreenMatchMode::Expand, "Expand");
    modeCombo->addChoice((int)ScreenMatchMode::Shrink, "Shrink");
    modeCombo->setEndOfLine(true);
    if (mode == ScreenMatchMode::MatchWidthOrHeight) {
        std::array matchValue = { canvas->getMatchWidthOrHeight() };
        m_canvasGroup->createWidget<Drag<float>>("Size", ImGuiDataType_Float, matchValue, 0.001f, 0, 1)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto canvas = std::dynamic_pointer_cast<Canvas>(getComponent<CompoundComponent>()->getComponents()[0]);
            canvas->setMatchWidthOrHeight(val[0]);
            });
    }
}
NS_IGE_END