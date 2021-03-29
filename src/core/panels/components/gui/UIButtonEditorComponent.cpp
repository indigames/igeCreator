#include "core/panels/components/gui/UIButtonEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UIButton.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

UIButtonEditorComponent::UIButtonEditorComponent() {
    m_uiButtonGroup = nullptr;
}

UIButtonEditorComponent::~UIButtonEditorComponent()
{
    if (m_uiButtonGroup) {
        m_uiButtonGroup->removeAllWidgets();
        m_uiButtonGroup->removeAllPlugins();
    }
    m_uiButtonGroup = nullptr;
}

bool UIButtonEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<UIButton*>(comp);
}

void UIButtonEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiButtonGroup == nullptr) {
        m_uiButtonGroup = m_group->createWidget<Group>("UIButtonGroup", false);
    }
    drawUIButton();

    EditorComponent::redraw();
}

void UIButtonEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiButtonGroup = m_group->createWidget<Group>("UIButtonGroup", false);

    drawUIButton();
}

void UIButtonEditorComponent::drawUIButton()
{
    if (m_uiButtonGroup == nullptr)
        return;
    m_uiButtonGroup->removeAllWidgets();

    auto uiButton = dynamic_cast<UIButton*>(m_component);
    if (uiButton == nullptr)
        return;

    auto m_interactable = m_uiButtonGroup->createWidget<CheckBox>("Interactable", uiButton->isInteractable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiButton = dynamic_cast<UIButton*>(m_component);
        uiButton->setInteractable(val);
        });

    auto transitionMode = uiButton->getTransitionMode();
    auto m_compComboTransitionMethod = m_uiButtonGroup->createWidget<ComboBox>((int)transitionMode);
    m_compComboTransitionMethod->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiButton = dynamic_cast<UIButton*>(m_component);
        uiButton->setTransitionMode((TransitionMode)val);
        dirty();
        });
    m_compComboTransitionMethod->setEndOfLine(false);
    m_compComboTransitionMethod->addChoice((int)TransitionMode::ColorTint, "Color Tint");
    m_compComboTransitionMethod->addChoice((int)TransitionMode::SpriteSwap, "Sprite Swap");
    //m_compComboTransitionMethod->addChoice((int)TransitionMode::Custom, "Custom");
    m_compComboTransitionMethod->setEndOfLine(true);

    if (transitionMode == TransitionMode::ColorTint) 
    {
        //Normal Path
        auto txtPath = m_uiButtonGroup->createWidget<TextField>("Image", uiButton->getPath());
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setTexturePath(txt, ButtonState::NORMAL);
            });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(txt, ButtonState::NORMAL);
                dirty();
                });
        }
        m_uiButtonGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
            if (files.size() > 0)
            {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(files[0], ButtonState::NORMAL);
                dirty();
            }
            });

        //! Normal Color
        m_uiButtonGroup->createWidget<Color>("Normal Color", uiButton->getColor())->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setColor(val[0], val[1], val[2], val[3]);
            });

        //! Pressed Color
        m_uiButtonGroup->createWidget<Color>("Pressed Color", uiButton->getPressedColor())->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setPressedColor(val[0], val[1], val[2], val[3]);
            });

        //! Selected Color
        m_uiButtonGroup->createWidget<Color>("Selected Color", uiButton->getSelectedColor())->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setSelectedColor(val[0], val[1], val[2], val[3]);
            });

        //! Disable Color
        auto color = uiButton->getColor();
        m_uiButtonGroup->createWidget<Color>("Disable Color", uiButton->getDisabledColor())->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setDisabledColor(val[0], val[1], val[2], val[3]);
            });

        //! Fade Duration
        std::array fadeDuration = { uiButton->getFadeDuration() };
        m_uiButtonGroup->createWidget<Drag<float>>("Fade Duration", ImGuiDataType_Float, fadeDuration, 0.01f, 0.0f, 60.0f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setFadeDuration(val[0]);
            });
    }
    else if (transitionMode == TransitionMode::SpriteSwap)
    {
        //Normal Path
        auto txtPath = m_uiButtonGroup->createWidget<TextField>("Normal", uiButton->getPath());
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setTexturePath(txt, ButtonState::NORMAL);
            });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(txt, ButtonState::NORMAL);
                dirty();
                });
        }
        m_uiButtonGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
            if (files.size() > 0)
            {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(files[0], ButtonState::NORMAL);
                dirty();
            }
            });

        //Pressed Path
        auto pressedPath = m_uiButtonGroup->createWidget<TextField>("Pressed", uiButton->getPressedPath());
        pressedPath->setEndOfLine(false);
        pressedPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setTexturePath(txt, ButtonState::PRESSED);
            });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            pressedPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(txt, ButtonState::PRESSED);
                dirty();
                });
        }
        m_uiButtonGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
            if (files.size() > 0)
            {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(files[0], ButtonState::PRESSED);
                dirty();
            }
            });

        //Selected Path
        auto selectedPath = m_uiButtonGroup->createWidget<TextField>("Selected", uiButton->getSelectedPath());
        selectedPath->setEndOfLine(false);
        selectedPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setTexturePath(txt, ButtonState::SELECTED);
            });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            selectedPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(txt, ButtonState::SELECTED);
                dirty();
                });
        }
        m_uiButtonGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
            if (files.size() > 0)
            {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(files[0], ButtonState::SELECTED);
                dirty();
            }
            });

        //Disabled Path
        auto disabledPath = m_uiButtonGroup->createWidget<TextField>("Disabled", uiButton->getDisabledPath());
        disabledPath->setEndOfLine(false);
        disabledPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setTexturePath(txt, ButtonState::DISABLE);
            });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            disabledPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(txt, ButtonState::DISABLE);
                dirty();
                });
        }
        m_uiButtonGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
            if (files.size() > 0)
            {
                auto uiButton = dynamic_cast<UIButton*>(m_component);
                uiButton->setTexturePath(files[0], ButtonState::DISABLE);
                dirty();
            }
            });

        auto color = uiButton->getColor();
        m_uiButtonGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiButton = dynamic_cast<UIButton*>(m_component);
            uiButton->setColor(val[0], val[1], val[2], val[3]);
            });
    }
}
NS_IGE_END