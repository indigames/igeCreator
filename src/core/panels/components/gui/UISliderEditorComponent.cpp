#include "core/panels/components/gui/UISliderEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UISlider.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

UISliderEditorComponent::UISliderEditorComponent() {
    m_uiSliderGroup = nullptr;
}

UISliderEditorComponent::~UISliderEditorComponent()
{
    if (m_uiSliderGroup) {
        m_uiSliderGroup->removeAllWidgets();
        m_uiSliderGroup->removeAllPlugins();
    }
    m_uiSliderGroup = nullptr;
}

bool UISliderEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<UISlider*>(comp);
}

void UISliderEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiSliderGroup == nullptr) {
        m_uiSliderGroup = m_group->createWidget<Group>("UISliderGroup", false);
    }
    drawUISlider();

    EditorComponent::redraw();
}

void UISliderEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiSliderGroup = m_group->createWidget<Group>("UISliderGroup", false);

    drawUISlider();
}

void UISliderEditorComponent::drawUISlider()
{
    if (m_uiSliderGroup == nullptr)
        return;
    m_uiSliderGroup->removeAllWidgets();

    auto uiSlider = dynamic_cast<UISlider*>(m_component);
    if (uiSlider == nullptr)
        return;

    auto m_interactable = m_uiSliderGroup->createWidget<CheckBox>("Interactable", uiSlider->isInteractable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiSlider = dynamic_cast<UISlider*>(m_component);
        uiSlider->setInteractable(val);
        });

    //! Normal Color
    m_uiSliderGroup->createWidget<Color>("Normal Color", uiSlider->getColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = dynamic_cast<UISlider*>(m_component);
        uiSlider->setColor(val[0], val[1], val[2], val[3]);
        });

    //! Pressed Color
    m_uiSliderGroup->createWidget<Color>("Pressed Color", uiSlider->getPressedColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = dynamic_cast<UISlider*>(m_component);
        uiSlider->setPressedColor(val[0], val[1], val[2], val[3]);
        });

    //! Disable Color
    m_uiSliderGroup->createWidget<Color>("Disable Color", uiSlider->getDisabledColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = dynamic_cast<UISlider*>(m_component);
        uiSlider->setDisabledColor(val[0], val[1], val[2], val[3]);
        });

    //! Fade Duration
    std::array fadeDuration = { uiSlider->getFadeDuration() };
    m_uiSliderGroup->createWidget<Drag<float>>("Fade Duration", ImGuiDataType_Float, fadeDuration, 0.01f, 0.0f, 60.0f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = dynamic_cast<UISlider*>(m_component);
        uiSlider->setFadeDuration(val[0]);
        });
}

NS_IGE_END