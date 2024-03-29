#include "core/scene/components/gui/UISliderEditorComponent.h"
#include "core/scene/CompoundComponent.h"

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

UISliderEditorComponent::~UISliderEditorComponent() {
    m_uiSliderMinGroup = nullptr;
    m_uiSliderMaxGroup = nullptr;
    m_uiSliderValueGroup = nullptr;
    m_uiSliderGroup = nullptr;
}

void UISliderEditorComponent::onInspectorUpdate() {
    switch (m_dirtyFlag) {
    case 1:
        drawMax();
        drawValue();
        break;
    case 2:
        drawMin();
        drawValue();
        break;
    default:
        drawUISlider();
        break;
    }
    m_dirtyFlag = 0;
}

void UISliderEditorComponent::drawUISlider() {
    if (m_uiSliderGroup == nullptr)
        m_uiSliderGroup = m_group->createWidget<Group>("UISliderGroup", false);;
    m_uiSliderGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_uiSliderMinGroup = nullptr;
    m_uiSliderMaxGroup = nullptr;
    m_uiSliderValueGroup = nullptr;

    m_uiSliderGroup->createWidget<CheckBox>("Interactable", comp->getProperty<bool>("interactable", true))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("interactable", val);
    });

    //! Normal Color
    m_uiSliderGroup->createWidget<Color>("Normal Color", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
    });

    //! Pressed Color
    m_uiSliderGroup->createWidget<Color>("Pressed Color", comp->getProperty<Vec4>("pressedcolor", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("pressedcolor", { val[0], val[1], val[2], val[3] });
    });

    //! Disable Color
    m_uiSliderGroup->createWidget<Color>("Disabled Color", comp->getProperty<Vec4>("disabledcolor", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("disabledcolor", { val[0], val[1], val[2], val[3] });
    });

    //! Fade Duration
    std::array fadeDuration = { comp->getProperty<float>("fadeduration", NAN) };
    m_uiSliderGroup->createWidget<Drag<float>>("Fade Duration", ImGuiDataType_Float, fadeDuration, 0.01f, 0.0f, 60.0f)->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("fadeduration", val[0]);
    });

    auto direction = comp->getProperty<int>("direction", -1);
    auto directionCombo = m_uiSliderGroup->createWidget<ComboBox>("Direction", (int)direction);
    directionCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("direction", val);
            setDirty();
        }
    });
    directionCombo->setEndOfLine(false);
    directionCombo->addChoice((int)UISlider::Direction::LeftToRight, "Left To Right");
    directionCombo->addChoice((int)UISlider::Direction::RightToLeft, "Right To Left");
    directionCombo->addChoice((int)UISlider::Direction::BottomToTop, "Bottom To Top");
    directionCombo->addChoice((int)UISlider::Direction::TopToBottom, "Top To Bottom");
    if(direction == -1) directionCombo->addChoice(-1, "");
    directionCombo->setEndOfLine(true);

    drawMin();
    drawMax();
    drawValue();

    m_uiSliderGroup->createWidget<CheckBox>("Whole Numbers", comp->getProperty<bool>("wholenumbers", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("wholenumbers", val);
    });
}

void UISliderEditorComponent::drawMin() {
    if (m_uiSliderMinGroup == nullptr)
        m_uiSliderMinGroup = m_uiSliderGroup->createWidget<Group>("UISliderMinGroup", false);;
    m_uiSliderMinGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    std::array min = { comp->getProperty<float>("min", NAN) };
    auto m = m_uiSliderMinGroup->createWidget<Drag<float>>("Min ", ImGuiDataType_Float, min, 0.01f, 0.0f, comp->getProperty<float>("max", 1.f));
    m->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    m->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("min", val[0]);
        getComponent<CompoundComponent>()->setDirty();
        auto value = getComponent<CompoundComponent>()->getProperty<float>("value", 0.f);
        if(value < val[0])
            getComponent<CompoundComponent>()->setProperty("value", val[0]);
        m_dirtyFlag = 1;
        setDirty();
    });
}

void UISliderEditorComponent::drawMax() {
    if (m_uiSliderMaxGroup == nullptr)
        m_uiSliderMaxGroup = m_uiSliderGroup->createWidget<Group>("UISliderMaxGroup", false);;
    m_uiSliderMaxGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    std::array max = { comp->getProperty<float>("max", NAN) };
    auto m = m_uiSliderMaxGroup->createWidget<Drag<float>>("Max ", ImGuiDataType_Float, max, 0.01f, comp->getProperty<float>("min", 0.f));
    m->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    m->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("max", val[0]);
        getComponent<CompoundComponent>()->setDirty();
        auto value = getComponent<CompoundComponent>()->getProperty<float>("value", 0.f);
        if (value > val[0])
            getComponent<CompoundComponent>()->setProperty("value", val[0]);
        m_dirtyFlag = 2;
        setDirty();
    });
}

void UISliderEditorComponent::drawValue() {
    if (m_uiSliderValueGroup == nullptr)
        m_uiSliderValueGroup = m_uiSliderGroup->createWidget<Group>("UISliderValGroup", false);;
    m_uiSliderValueGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto minVal = comp->getProperty<float>("min", 0.f);
    auto maxVal = comp->getProperty<float>("max", 1.f);
    auto step = (maxVal - minVal) / 100.f;
    std::array value = { comp->getProperty<float>("value", NAN) };

    auto m = m_uiSliderValueGroup->createWidget<Drag<float>>("Value ", ImGuiDataType_Float, value, step, minVal, maxVal);
    m->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    m->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("value", val[0]);
        getComponent<CompoundComponent>()->setDirty();
        m_dirtyFlag = 0;
    });
}
NS_IGE_END