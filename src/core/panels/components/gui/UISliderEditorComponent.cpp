#include "core/panels/components/gui/UISliderEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UISlider.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

UISliderEditorComponent::UISliderEditorComponent() {
    m_uiSliderGroup = nullptr;
    m_uiSliderMinGroup = nullptr;
    m_uiSliderMaxGroup = nullptr;
    m_uiSliderValueGroup = nullptr;
    m_dirtyFlag = 0;
}

UISliderEditorComponent::~UISliderEditorComponent()
{
    m_uiSliderMinGroup = nullptr;
    m_uiSliderMaxGroup = nullptr;
    m_uiSliderValueGroup = nullptr;
    m_uiSliderGroup = nullptr;
}

void UISliderEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiSliderGroup == nullptr) {
        m_uiSliderGroup = m_group->createWidget<Group>("UISliderGroup", false);
    }
    switch (m_dirtyFlag) {
    case 1 :
        drawMax();
        drawValue();
        break;
    case 2 :
        drawMin();
        drawValue();
        break;
    default:
        drawUISlider();
        break;
    }
    m_dirtyFlag = 0;

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

    auto uiSlider = getComponent<UISlider>();
    if (uiSlider == nullptr)
        return;

    m_uiSliderMinGroup = nullptr;
    m_uiSliderMaxGroup = nullptr;
    m_uiSliderValueGroup = nullptr;

    auto m_interactable = m_uiSliderGroup->createWidget<CheckBox>("Interactable", uiSlider->isInteractable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiSlider = getComponent<UISlider>();
        uiSlider->setInteractable(val);
        });

    //! Normal Color
    m_uiSliderGroup->createWidget<Color>("Normal Color", uiSlider->getColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = getComponent<UISlider>();
        uiSlider->setColor(val[0], val[1], val[2], val[3]);
        });

    //! Pressed Color
    m_uiSliderGroup->createWidget<Color>("Pressed Color", uiSlider->getPressedColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = getComponent<UISlider>();
        uiSlider->setPressedColor(val[0], val[1], val[2], val[3]);
        });

    //! Disable Color
    m_uiSliderGroup->createWidget<Color>("Disable Color", uiSlider->getDisabledColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = getComponent<UISlider>();
        uiSlider->setDisabledColor(val[0], val[1], val[2], val[3]);
        });

    //! Fade Duration
    std::array fadeDuration = { uiSlider->getFadeDuration() };
    m_uiSliderGroup->createWidget<Drag<float>>("Fade Duration", ImGuiDataType_Float, fadeDuration, 0.01f, 0.0f, 60.0f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = getComponent<UISlider>();
        uiSlider->setFadeDuration(val[0]);
        });

    auto direction = uiSlider->getDirection();
    auto m_directionCombo = m_uiSliderGroup->createWidget<ComboBox>((int)direction);
    m_directionCombo->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = getComponent<UISlider>();
        uiSlider->setDirection(val);
        dirty();
        });
    m_directionCombo->setEndOfLine(false);
    m_directionCombo->addChoice((int)UISlider::Direction::LEFT_TO_RIGHT, "Left To Right");
    m_directionCombo->addChoice((int)UISlider::Direction::RIGHT_TO_LEFT, "Right To Left");
    m_directionCombo->addChoice((int)UISlider::Direction::BOTTOM_TO_TOP, "Bottom To Top");
    m_directionCombo->addChoice((int)UISlider::Direction::TOP_TO_BOTTOM, "Top To Bottom");
    m_directionCombo->setEndOfLine(true);

    m_uiSliderMinGroup = m_uiSliderGroup->createWidget<Group>("UISliderGroup", false);

    m_uiSliderMaxGroup = m_uiSliderGroup->createWidget<Group>("UISliderGroup", false);

    m_uiSliderValueGroup = m_uiSliderGroup->createWidget<Group>("UISliderGroup", false);

    drawMin();
    drawMax();
    drawValue();

    m_uiSliderGroup->createWidget<CheckBox>("Whole Numbers", uiSlider->getWholeNumbers())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiSlider = getComponent<UISlider>();
        uiSlider->setWholeNumbers(val);
        });
}

void UISliderEditorComponent::drawMin()
{
    if (m_uiSliderMinGroup == nullptr)
        return;
    m_uiSliderMinGroup->removeAllWidgets();

    auto uiSlider = getComponent<UISlider>();
    if (uiSlider == nullptr)
        return;

    auto m_min = uiSlider->getMin();
    auto m_max = uiSlider->getMax();
    auto m_value = uiSlider->getValue();

    std::array min = { uiSlider->getMin() };
    m_uiSliderMinGroup->createWidget<Drag<float>>("Min ", ImGuiDataType_Float, min, 0.01f, 0.0f, m_max)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = getComponent<UISlider>();
        auto max = uiSlider->getMax();
        if (val[0] < max) {
            uiSlider->setMin(val[0]);
            dirty();
            m_dirtyFlag = 1;
        }
        });
}

void UISliderEditorComponent::drawMax()
{
    if (m_uiSliderMaxGroup == nullptr)
        return;
    m_uiSliderMaxGroup->removeAllWidgets();

    auto uiSlider = getComponent<UISlider>();
    if (uiSlider == nullptr)
        return;

    auto m_min = uiSlider->getMin();
    auto m_max = uiSlider->getMax();
    auto m_value = uiSlider->getValue();

    std::array max = { uiSlider->getMax() };
    m_uiSliderMaxGroup->createWidget<Drag<float>>("Max ", ImGuiDataType_Float, max, 0.01f, m_min, 100.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = getComponent<UISlider>();
        auto min = uiSlider->getMin();
        if (val[0] > min) {
            uiSlider->setMax(val[0]);
            dirty();
            m_dirtyFlag = 2;
        }
        });
}

void UISliderEditorComponent::drawValue()
{
    if (m_uiSliderValueGroup == nullptr)
        return;
    m_uiSliderValueGroup->removeAllWidgets();

    auto uiSlider = getComponent<UISlider>();
    if (uiSlider == nullptr)
        return;

    auto m_min = uiSlider->getMin();
    auto m_max = uiSlider->getMax();
    auto m_value = uiSlider->getValue();

    std::array value = { uiSlider->getValue() };
    m_uiSliderValueGroup->createWidget<Drag<float>>("Value ", ImGuiDataType_Float, value, 0.01f, m_min, m_max)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiSlider = getComponent<UISlider>();
        auto min = uiSlider->getMin();
        auto max = uiSlider->getMax();
        if (val[0] >= min && val[0] <= max)
            uiSlider->setValue(val[0]);
        });
}
NS_IGE_END