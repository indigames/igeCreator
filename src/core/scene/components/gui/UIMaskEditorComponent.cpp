#include "core/scene/components/gui/UIMaskEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UIMask.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

UIMaskEditorComponent::UIMaskEditorComponent() {
    m_uiMaskGroup = nullptr;
}

UIMaskEditorComponent::~UIMaskEditorComponent() {
    m_uiMaskGroup = nullptr;
}

void UIMaskEditorComponent::onInspectorUpdate() {
    drawUIMask();
}

void UIMaskEditorComponent::drawUIMask() {
    if (m_uiMaskGroup == nullptr)
        m_uiMaskGroup = m_group->createWidget<Group>("UIMaskGroup", false);;
    m_uiMaskGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtPath = m_uiMaskGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""), false, true);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        getComponent<CompoundComponent>()->setProperty("path", txt);
        getComponent<CompoundComponent>()->setDirty();
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite)) {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            getComponent<CompoundComponent>()->setProperty("path", txt);
            getComponent<CompoundComponent>()->setDirty();
            setDirty();
        });
    }

    m_uiMaskGroup->createWidget<CheckBox>("Use Mask", comp->getProperty<bool>("usemask", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("usemask", val);
    });

    m_uiMaskGroup->createWidget<CheckBox>("Interactable", comp->getProperty<bool>("interactable", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("interactable", val);
    });

    auto spriteType = comp->getProperty<int>("spritetype", -1);
    auto spriteTypeCombo = m_uiMaskGroup->createWidget<ComboBox>("Sprite Type", spriteType);
    spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            getComponent<CompoundComponent>()->setProperty("spritetype", val);
            setDirty();
        }
    });
    spriteTypeCombo->setEndOfLine(false);
    spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    if (spriteType == -1) spriteTypeCombo->addChoice(-1, "");
    spriteTypeCombo->setEndOfLine(true);

    if (spriteType == (int)SpriteType::Sliced) {
        auto border = comp->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
        std::array borderLeft = { border.X() };
        m_uiMaskGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.X(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", val);
        });
        std::array borderRight = { border.Y() };
        m_uiMaskGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.Y(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", val);
        });
        std::array borderTop = { border.Z() };
        m_uiMaskGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.Z(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", val);
        });
        std::array borderBottom = { border.W() };
        m_uiMaskGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.W(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", val);
        });
    }
    else {
        auto fillMethod = comp->getProperty<int>("fillmethod", -1);
        auto fillMethodCombo = m_uiMaskGroup->createWidget<ComboBox>("Fill Method", (int)fillMethod);
        fillMethodCombo->getOnDataChangedEvent().addListener([this](auto val) {
            if (val != -1) {
                getComponent<CompoundComponent>()->setProperty("fillmethod", val);
                setDirty();
            }
        });
        fillMethodCombo->setEndOfLine(false);
        fillMethodCombo->addChoice((int)FillMethod::None, "None");
        fillMethodCombo->addChoice((int)FillMethod::Horizontal, "Horizontal");
        fillMethodCombo->addChoice((int)FillMethod::Vertical, "Vertical");
        fillMethodCombo->addChoice((int)FillMethod::Radial90, "Radial 90");
        fillMethodCombo->addChoice((int)FillMethod::Radial180, "Radial 180");
        fillMethodCombo->addChoice((int)FillMethod::Radial360, "Radial 360");
        if (fillMethod == -1) fillMethodCombo->addChoice(-1, "");
        fillMethodCombo->setEndOfLine(true);
        if (fillMethod != (int)FillMethod::None && fillMethod != -1) {
            auto fillOrigin = comp->getProperty<int>("fillorigin", -1);
            auto fillOriginCombo = m_uiMaskGroup->createWidget<ComboBox>("Fill Origin", fillOrigin);
            fillOriginCombo->getOnDataChangedEvent().addListener([this](auto val) {
                if (val != -1) {
                    getComponent<CompoundComponent>()->setProperty("fillorigin", val);
                    setDirty();
                }
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
            if (fillOrigin == -1) fillOriginCombo->addChoice(-1, "");
            fillOriginCombo->setEndOfLine(true);

            std::array fillAmount = { comp->getProperty<float>("fillamount", NAN) };
            m_uiMaskGroup->createWidget<Drag<float, 1>>("Fill Amount", ImGuiDataType_Float, fillAmount, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                getComponent<CompoundComponent>()->setProperty("fillamount", val[0]);
            });

            if (fillMethod == (int)FillMethod::Radial90 || fillMethod == (int)FillMethod::Radial180 || fillMethod == (int)FillMethod::Radial360) {
                m_uiMaskGroup->createWidget<CheckBox>("Clockwise", comp->getProperty<bool>("clockwise", false))->getOnDataChangedEvent().addListener([this](bool val) {
                    getComponent<CompoundComponent>()->setProperty("clockwise", val);
                });
            }
        }
    }
    m_uiMaskGroup->createWidget<Color>("Color", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
    });
}
NS_IGE_END