#include "core/scene/components/gui/UIScrollBarEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UIScrollBar.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

UIScrollBarEditorComponent::UIScrollBarEditorComponent() {
    m_uiScrollBarGroup = nullptr;
}

UIScrollBarEditorComponent::~UIScrollBarEditorComponent() {
    m_uiScrollBarGroup = nullptr;
}

void UIScrollBarEditorComponent::onInspectorUpdate() {
    drawUIScrollBar();
}

void UIScrollBarEditorComponent::drawUIScrollBar() {
    if (m_uiScrollBarGroup == nullptr)
        m_uiScrollBarGroup = m_group->createWidget<Group>("UIScrollBarGroup", false);
    m_uiScrollBarGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtPath = m_uiScrollBarGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""));
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        getComponent<CompoundComponent>()->setProperty("path", txt);
    });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite)) {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            getComponent<CompoundComponent>()->setProperty("path", txt);
            setDirty();
        });
    }

    m_uiScrollBarGroup->createWidget<CheckBox>("Interactable", comp->getProperty<bool>("interactable", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("interactable", val);
    });

    auto spriteType = comp->getProperty<int>("spritetype", 0);
    auto spriteTypeCombo = m_uiScrollBarGroup->createWidget<ComboBox>("Sprite Type", spriteType);
    spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("spritetype", val);
        setDirty();
    });
    spriteTypeCombo->setEndOfLine(false);
    spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    spriteTypeCombo->setEndOfLine(true);

    if (spriteType == (int)SpriteType::Sliced) {
        auto border = comp->getProperty<Vec4>("border", {});
        std::array borderLeft = { border.X() };
        m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", {});
            border.X(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", val);
        });
        std::array borderRight = { border.Y() };
        m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", {});
            border.Y(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", val);
        });
        std::array borderTop = { border.Z() };
        m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", {});
            border.Z(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", val);
        });
        std::array borderBottom = { border.W() };
        m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", {});
            border.W(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", val);
        });
    }
    else {
        auto fillMethod = comp->getProperty<int>("fillmethod", 0);
        auto fillMethodCombo = m_uiScrollBarGroup->createWidget<ComboBox>("Fill Method", (int)fillMethod);
        fillMethodCombo->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("fillmethod", val);
            setDirty();
        });
        fillMethodCombo->setEndOfLine(false);
        fillMethodCombo->addChoice((int)FillMethod::None, "None");
        fillMethodCombo->addChoice((int)FillMethod::Horizontal, "Horizontal");
        fillMethodCombo->addChoice((int)FillMethod::Vertical, "Vertical");
        fillMethodCombo->addChoice((int)FillMethod::Radial90, "Radial 90");
        fillMethodCombo->addChoice((int)FillMethod::Radial180, "Radial 180");
        fillMethodCombo->addChoice((int)FillMethod::Radial360, "Radial 360");
        fillMethodCombo->setEndOfLine(true);

        if (fillMethod != (int)FillMethod::None) {
            auto fillOriginCombo = m_uiScrollBarGroup->createWidget<ComboBox>("Fill Origin", comp->getProperty<int>("fillorigin", 0));
            fillOriginCombo->getOnDataChangedEvent().addListener([this](auto val) {
                getComponent<CompoundComponent>()->setProperty("fillorigin", val);
                setDirty();
            });
            fillOriginCombo->setEndOfLine(false);
            if (fillMethod == (int)FillMethod::Horizontal) {
                fillOriginCombo->addChoice((int)FillOrigin::Left, "Left");
                fillOriginCombo->addChoice((int)FillOrigin::Right, "Right");
            }
            else if (fillMethod == (int)FillMethod::Vertical) {
                fillOriginCombo->addChoice((int)FillOrigin::Top, "Top");
                fillOriginCombo->addChoice((int)FillOrigin::Bottom, "Bottom");
            }
            else if (fillMethod == (int)FillMethod::Radial90) {
                fillOriginCombo->addChoice((int)FillOrigin::BottomLeft, "Bottom Left");
                fillOriginCombo->addChoice((int)FillOrigin::TopLeft, "Top Left");
                fillOriginCombo->addChoice((int)FillOrigin::TopRight, "Top Right");
                fillOriginCombo->addChoice((int)FillOrigin::BottomRight, "Bottom Right");
            }
            else {
                fillOriginCombo->addChoice((int)FillOrigin::Bottom, "Bottom");
                fillOriginCombo->addChoice((int)FillOrigin::Top, "Top");
                fillOriginCombo->addChoice((int)FillOrigin::Left, "Left");
                fillOriginCombo->addChoice((int)FillOrigin::Right, "Right");
            }
            fillOriginCombo->setEndOfLine(true);

            std::array fillAmount = { comp->getProperty<float>("fillamount", 0.f) };
            m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Fill Amount", ImGuiDataType_Float, fillAmount, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                getComponent<CompoundComponent>()->setProperty("fillamount", val[0]);
            });

            if (fillMethod == (int)FillMethod::Radial90 || fillMethod == (int)FillMethod::Radial180 || fillMethod == (int)FillMethod::Radial360) {
                m_uiScrollBarGroup->createWidget<CheckBox>("Clockwise", comp->getProperty<bool>("clockwise", false))->getOnDataChangedEvent().addListener([this](bool val) {
                    getComponent<CompoundComponent>()->setProperty("clockwise", val);
                });
            }
        }
    }
    m_uiScrollBarGroup->createWidget<Color>("Color", comp->getProperty<Vec4>("color", {}))->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
    });

    //! Normal Color
    m_uiScrollBarGroup->createWidget<Color>("Normal Color", comp->getProperty<Vec4>("normalcolor", {}))->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("normalcolor", { val[0], val[1], val[2], val[3] });
    });

    //! Pressed Color
    m_uiScrollBarGroup->createWidget<Color>("Pressed Color", comp->getProperty<Vec4>("pressedcolor", {}))->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("pressedcolor", { val[0], val[1], val[2], val[3] });
    });

    //! Disable Color
    m_uiScrollBarGroup->createWidget<Color>("Disabled Color", comp->getProperty<Vec4>("disabledcolor", {}))->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("disabledcolor", { val[0], val[1], val[2], val[3] });
    });

    //! Fade Duration
    std::array fadeDuration = { comp->getProperty<float>("fadeduration", 0.f) };
    m_uiScrollBarGroup->createWidget<Drag<float>>("Fade Duration", ImGuiDataType_Float, fadeDuration, 0.01f, 0.0f, 60.0f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("fadeduration", val[0]);
    });

    auto direction = comp->getProperty<int>("directionbar", 0);
    auto directionCombo = m_uiScrollBarGroup->createWidget<ComboBox>("Direction", (int)direction);
    directionCombo->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("directionbar", val);
        setDirty();
    });
    directionCombo->setEndOfLine(false);
    directionCombo->addChoice((int)UIScrollBar::Direction::LeftToRight, "Left To Right");
    directionCombo->addChoice((int)UIScrollBar::Direction::RightToLeft, "Right To Left");
    directionCombo->addChoice((int)UIScrollBar::Direction::BottomToTop, "Bottom To Top");
    directionCombo->addChoice((int)UIScrollBar::Direction::TopToBottom, "Top To Bottom");
    directionCombo->setEndOfLine(true);

    std::array value = { comp->getProperty<float>("valuebar", 0.f) };
    m_uiScrollBarGroup->createWidget<Drag<float>>("Value", ImGuiDataType_Float, value, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("valuebar", val[0]);
    });

    std::array size = { comp->getProperty<float>("sizebar", 0.f) };
    m_uiScrollBarGroup->createWidget<Drag<float>>("Size", ImGuiDataType_Float, size, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("sizebar", val[0]);
    });
}
NS_IGE_END