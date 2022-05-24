#include "core/scene/components/gui/UIButtonEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "core/Editor.h"

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

UIButtonEditorComponent::~UIButtonEditorComponent() {
    m_uiButtonGroup = nullptr;
}


void UIButtonEditorComponent::onInspectorUpdate() {
    drawUIButton();
}

void UIButtonEditorComponent::drawUIButton() {
    if (m_uiButtonGroup == nullptr)
        m_uiButtonGroup = m_group->createWidget<Group>("UIButtonGroup", false);;
    m_uiButtonGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_uiButtonGroup->createWidget<CheckBox>("Interactable", comp->getProperty<bool>("interactable", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("interactable", val);
    });

    auto transitionMode = comp->getProperty<int>("transitionmode", -1);
    auto transitionMethodCombo = m_uiButtonGroup->createWidget<ComboBox>("Transition Mode", transitionMode);
    transitionMethodCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("transitionmode", val);
            setDirty();
        }
    });
    transitionMethodCombo->setEndOfLine(false);
    transitionMethodCombo->addChoice((int)TransitionMode::ColorTint, "Color Tint");
    transitionMethodCombo->addChoice((int)TransitionMode::SpriteSwap, "Sprite Swap");
    if(transitionMode == -1) transitionMethodCombo->addChoice(-1, "");
    transitionMethodCombo->setEndOfLine(true);

    if (transitionMode == (int)TransitionMode::ColorTint) {
        //Normal Path
        auto txtPath = m_uiButtonGroup->createWidget<TextField>("Image", comp->getProperty<std::string>("path", ""), false, true);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("path", txt);
            getComponent<CompoundComponent>()->setDirty();
        });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite)) {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                storeUndo();
                getComponent<CompoundComponent>()->setProperty("path", GetRelativePath(path));
                getComponent<CompoundComponent>()->setDirty();
                setDirty();
            });
        }

        //! Normal Color
        m_uiButtonGroup->createWidget<Color>("Normal", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
        });

        //! Pressed Color
        m_uiButtonGroup->createWidget<Color>("Pressed", comp->getProperty<Vec4>("pressedcolor", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("pressedcolor", { val[0], val[1], val[2], val[3] });
        });

        //! Selected Color
        m_uiButtonGroup->createWidget<Color>("Selected", comp->getProperty<Vec4>("selectedcolor", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("selectedcolor", { val[0], val[1], val[2], val[3] });
        });

        //! Disable Color
        m_uiButtonGroup->createWidget<Color>("Disabled", comp->getProperty<Vec4>("disabledcolor", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("disabledcolor", { val[0], val[1], val[2], val[3] });
        });

        //! Fade Duration
        std::array fadeDuration = { comp->getProperty<float>("fadeduration", NAN) };
        auto f1 = m_uiButtonGroup->createWidget<Drag<float>>("Fade Duration", ImGuiDataType_Float, fadeDuration, 0.01f, 0.0f, 60.0f);
        f1->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        f1->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("fadeduration", val[0]);
        });
    } else if (transitionMode == (int)TransitionMode::SpriteSwap) {
        //Normal Path
        auto txtPath = m_uiButtonGroup->createWidget<TextField>("Normal", comp->getProperty<std::string>("path", ""), false, true);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("path", txt);
        });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite)) {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                storeUndo();
                getComponent<CompoundComponent>()->setProperty("path", GetRelativePath(path));
                setDirty();
            });
        }

        //Pressed Path
        txtPath = m_uiButtonGroup->createWidget<TextField>("Pressed", comp->getProperty<std::string>("pressedpath", ""), false, true);
        txtPath->getOnDataChangedEvent().addListener([this](const auto& path) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("pressedpath", GetRelativePath(path));
        });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite)) {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                storeUndo();
                getComponent<CompoundComponent>()->setProperty("pressedpath", GetRelativePath(path));
                setDirty();
            });
        }

        //Selected Path
        txtPath = m_uiButtonGroup->createWidget<TextField>("Selected", comp->getProperty<std::string>("seletecpath", ""), false, true);
        txtPath->getOnDataChangedEvent().addListener([this](const auto& path) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("seletecpath", GetRelativePath(path));
        });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite)) {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                storeUndo();
                getComponent<CompoundComponent>()->setProperty("seletecpath", GetRelativePath(path));
                setDirty();
            });
        }

        //Disabled Path
        txtPath = m_uiButtonGroup->createWidget<TextField>("Disabled", comp->getProperty<std::string>("disabledpath", ""), false, true);
        txtPath->getOnDataChangedEvent().addListener([this](const auto& path) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("disabledpath", GetRelativePath(path));
        });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite)) {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                storeUndo();
                getComponent<CompoundComponent>()->setProperty("disabledpath", GetRelativePath(path));
                setDirty();
            });
        }

        m_uiButtonGroup->createWidget<Color>("Color", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
        });
    }

    auto spriteType = comp->getProperty<int>("spritetype", -1);
    auto spriteTypeCombo = m_uiButtonGroup->createWidget<ComboBox>("Sprite Type", spriteType);
    spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("spritetype", val);
            setDirty();
        }
    });
    spriteTypeCombo->setEndOfLine(false);
    spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    if(spriteType == -1) spriteTypeCombo->addChoice(-1, "");
    spriteTypeCombo->setEndOfLine(true);

    if (spriteType == (int)SpriteType::Sliced) {
        auto border = comp->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
        std::array borderLeft = { border.X() };
        auto b1 = m_uiButtonGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f);
        b1->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        b1->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.X(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", border);
        });
        std::array borderRight = { border.Y() };
        auto b2 = m_uiButtonGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f);
        b2->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        b2->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.Y(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", border);
        });
        std::array borderTop = { border.Z() };
        auto b3 = m_uiButtonGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f);
        b3->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        b3->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.Z(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", border);
        });
        std::array borderBottom = { border.W() };
        auto b4 = m_uiButtonGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f);
        b4->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        b4->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.W(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", border);
        });
    }
}
NS_IGE_END
