#include "core/scene/components/gui/UIScrollBarEditorComponent.h"

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

UIScrollBarEditorComponent::~UIScrollBarEditorComponent()
{
    m_uiScrollBarGroup = nullptr;
}

void UIScrollBarEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiScrollBarGroup == nullptr) {
        m_uiScrollBarGroup = m_group->createWidget<Group>("UIScrollBarGroup", false);
    }
    drawUIScrollBar();

    EditorComponent::redraw();
}

void UIScrollBarEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiScrollBarGroup = m_group->createWidget<Group>("UIScrollBarGroup", false);

    drawUIScrollBar();
}

void UIScrollBarEditorComponent::drawUIScrollBar()
{
    if (m_uiScrollBarGroup == nullptr)
        return;
    m_uiScrollBarGroup->removeAllWidgets();

    auto uiScrollBar = getComponent<UIScrollBar>();
    if (uiScrollBar == nullptr)
        return;

    auto txtPath = m_uiScrollBarGroup->createWidget<TextField>("Path", uiScrollBar->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiImage = getComponent<UIScrollBar>();
        uiImage->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto uiScrollBar = getComponent<UIScrollBar>();
            uiScrollBar->setPath(txt);
            setDirty();
            });
    }

    m_uiScrollBarGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
        if (files.size() > 0)
        {
            auto uiScrollBar =getComponent<UIScrollBar>();
            uiScrollBar->setPath(files[0]);
            setDirty();
        }
        });

    auto m_interactable = m_uiScrollBarGroup->createWidget<CheckBox>("Interactable", uiScrollBar->isInteractable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setInteractable(val);
        });

    auto spriteType = uiScrollBar->getSpriteType();
    auto m_spriteTypeCombo = m_uiScrollBarGroup->createWidget<ComboBox>("", (int)spriteType);
    m_spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setSpriteType(val);
        setDirty();
        });
    m_spriteTypeCombo->setEndOfLine(false);
    m_spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    m_spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    m_spriteTypeCombo->setEndOfLine(true);


    if (spriteType == SpriteType::Sliced) {
        std::array borderLeft = { uiScrollBar->getBorderLeft() };
        m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollBar =getComponent<UIScrollBar>();
            uiScrollBar->setBorderLeft(val[0]);
            });
        std::array borderRight = { uiScrollBar->getBorderRight() };
        m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollBar =getComponent<UIScrollBar>();
            uiScrollBar->setBorderRight(val[0]);
            });
        std::array borderTop = { uiScrollBar->getBorderTop() };
        m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollBar =getComponent<UIScrollBar>();
            uiScrollBar->setBorderTop(val[0]);
            });
        std::array borderBottom = { uiScrollBar->getBorderBottom() };
        m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollBar =getComponent<UIScrollBar>();
            uiScrollBar->setBorderBottom(val[0]);
            });
    }
    else
    {
        auto fillMethod = uiScrollBar->getFillMethod();
        auto m_compComboFillMethod = m_uiScrollBarGroup->createWidget<ComboBox>("", (int)fillMethod);
        m_compComboFillMethod->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollBar =getComponent<UIScrollBar>();
            uiScrollBar->setFillMethod(val);
            setDirty();
            });
        m_compComboFillMethod->setEndOfLine(false);
        m_compComboFillMethod->addChoice((int)FillMethod::None, "None");
        m_compComboFillMethod->addChoice((int)FillMethod::Horizontal, "Horizontal");
        m_compComboFillMethod->addChoice((int)FillMethod::Vertical, "Vertical");
        m_compComboFillMethod->addChoice((int)FillMethod::Radial90, "Radial 90");
        m_compComboFillMethod->addChoice((int)FillMethod::Radial180, "Radial 180");
        m_compComboFillMethod->addChoice((int)FillMethod::Radial360, "Radial 360");
        m_compComboFillMethod->setEndOfLine(true);

        if (fillMethod != FillMethod::None) {

            auto m_compComboFillOrigin = m_uiScrollBarGroup->createWidget<ComboBox>("", (int)uiScrollBar->getFillOrigin());
            m_compComboFillOrigin->getOnDataChangedEvent().addListener([this](auto val) {
                auto uiScrollBar =getComponent<UIScrollBar>();
                uiScrollBar->setFillOrigin(val);
                setDirty();
                });
            m_compComboFillOrigin->setEndOfLine(false);
            if (fillMethod == FillMethod::Horizontal)
            {
                m_compComboFillOrigin->addChoice((int)FillOrigin::Left, "Left");
                m_compComboFillOrigin->addChoice((int)FillOrigin::Right, "Right");
            }
            else if (fillMethod == FillMethod::Vertical)
            {
                m_compComboFillOrigin->addChoice((int)FillOrigin::Top, "Top");
                m_compComboFillOrigin->addChoice((int)FillOrigin::Bottom, "Bottom");
            }
            else if (fillMethod == FillMethod::Radial90)
            {
                m_compComboFillOrigin->addChoice((int)FillOrigin::BottomLeft, "Bottom Left");
                m_compComboFillOrigin->addChoice((int)FillOrigin::TopLeft, "Top Left");
                m_compComboFillOrigin->addChoice((int)FillOrigin::TopRight, "Top Right");
                m_compComboFillOrigin->addChoice((int)FillOrigin::BottomRight, "Bottom Right");
            }
            else
            {
                m_compComboFillOrigin->addChoice((int)FillOrigin::Bottom, "Bottom");
                m_compComboFillOrigin->addChoice((int)FillOrigin::Top, "Top");
                m_compComboFillOrigin->addChoice((int)FillOrigin::Left, "Left");
                m_compComboFillOrigin->addChoice((int)FillOrigin::Right, "Right");
            }
            m_compComboFillOrigin->setEndOfLine(true);

            std::array fillAmount = { uiScrollBar->getFillAmount() };
            m_uiScrollBarGroup->createWidget<Drag<float, 1>>("Fill Amount", ImGuiDataType_Float, fillAmount, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto uiScrollBar =getComponent<UIScrollBar>();
                uiScrollBar->setFillAmount(val[0]);
                });

            if (fillMethod == FillMethod::Radial90 || fillMethod == FillMethod::Radial180 || fillMethod == FillMethod::Radial360) {
                m_uiScrollBarGroup->createWidget<CheckBox>("Clockwise", uiScrollBar->getClockwise())->getOnDataChangedEvent().addListener([this](bool val) {
                    auto uiScrollBar =getComponent<UIScrollBar>();
                    uiScrollBar->setClockwise(val);
                    });
            }
        }
    }

    auto color = uiScrollBar->getColor();
    m_uiScrollBarGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setColor(val[0], val[1], val[2], val[3]);
        });

    //! Normal Color
    m_uiScrollBarGroup->createWidget<Color>("Normal Color", uiScrollBar->getNormalColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setNormalColor(val[0], val[1], val[2], val[3]);
        });

    //! Pressed Color
    m_uiScrollBarGroup->createWidget<Color>("Pressed Color", uiScrollBar->getPressedColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setPressedColor(val[0], val[1], val[2], val[3]);
        });

    //! Disable Color
    m_uiScrollBarGroup->createWidget<Color>("Disable Color", uiScrollBar->getDisabledColor())->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setDisabledColor(val[0], val[1], val[2], val[3]);
        });

    //! Fade Duration
    std::array fadeDuration = { uiScrollBar->getFadeDuration() };
    m_uiScrollBarGroup->createWidget<Drag<float>>("Fade Duration", ImGuiDataType_Float, fadeDuration, 0.01f, 0.0f, 60.0f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setFadeDuration(val[0]);
        });


    auto direction = uiScrollBar->getDirection();
    auto m_compComboDirection = m_uiScrollBarGroup->createWidget<ComboBox>("", (int)direction);
    m_compComboDirection->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setDirection(val);
        setDirty();
        });

    m_compComboDirection->setEndOfLine(false);
    m_compComboDirection->addChoice((int)UIScrollBar::Direction::LeftToRight, "Left To Right");
    m_compComboDirection->addChoice((int)UIScrollBar::Direction::RightToLeft, "Right To Left");
    m_compComboDirection->addChoice((int)UIScrollBar::Direction::BottomToTop, "Bottom To Top");
    m_compComboDirection->addChoice((int)UIScrollBar::Direction::TopToBottom, "Top To Bottom");
    m_compComboDirection->setEndOfLine(true);

    std::array value = { uiScrollBar->getValue () };
    m_uiScrollBarGroup->createWidget<Drag<float>>("Value", ImGuiDataType_Float, value, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setValue(val[0]);
        });
    std::array size = { uiScrollBar->getSize() };
    m_uiScrollBarGroup->createWidget<Drag<float>>("Size", ImGuiDataType_Float, size, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setSize(val[0]);
        });
    /*std::array step = { uiScrollBar->getStep() };
    m_uiScrollBarGroup->createWidget<Drag<float>>("Deceleration Rate", ImGuiDataType_Float, value, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollBar =getComponent<UIScrollBar>();
        uiScrollBar->setValue(val[0]);
        });*/
}
NS_IGE_END