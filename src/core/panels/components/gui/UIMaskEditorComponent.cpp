#include "core/panels/components/gui/UIMaskEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UIMask.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

UIMaskEditorComponent::UIMaskEditorComponent() 
{
    m_uiMaskGroup = nullptr;
}

UIMaskEditorComponent::~UIMaskEditorComponent()
{
    if (m_uiMaskGroup) {
        m_uiMaskGroup->removeAllWidgets();
        m_uiMaskGroup->removeAllPlugins();
        m_uiMaskGroup = nullptr;
    }
}

bool UIMaskEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<UIMask*>(comp);
}

void UIMaskEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiMaskGroup == nullptr) {
        m_uiMaskGroup = m_group->createWidget<Group>("UIMaskGroup", false);
    }
    drawUIMask();

    EditorComponent::redraw();
}

void UIMaskEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiMaskGroup = m_group->createWidget<Group>("UIMaskGroup", false);

    drawUIMask();
}

void UIMaskEditorComponent::drawUIMask()
{
    if (m_uiMaskGroup == nullptr)
        return;
    m_uiMaskGroup->removeAllWidgets();

    auto uiMask = dynamic_cast<UIMask*>(m_component);
    if (uiMask == nullptr)
        return;

    auto txtPath = m_uiMaskGroup->createWidget<TextField>("Path", uiMask->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiImage = dynamic_cast<UIMask*>(m_component);
        uiImage->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto uiMask = dynamic_cast<UIMask*>(m_component);
            uiMask->setPath(txt);
            dirty();
            });
    }

    m_uiMaskGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
        if (files.size() > 0)
        {
            auto uiMask = dynamic_cast<UIMask*>(m_component);
            uiMask->setPath(files[0]);
            dirty();
        }
        });

    auto m_useMask = m_uiMaskGroup->createWidget<CheckBox>("Use Mask", uiMask->isUseMask())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiMask = dynamic_cast<UIMask*>(m_component);
        uiMask->setUseMask(val);
        });

    auto m_interactable = m_uiMaskGroup->createWidget<CheckBox>("Interactable", uiMask->isInteractable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiMask = dynamic_cast<UIMask*>(m_component);
        uiMask->setInteractable(val);
        });

    auto spriteType = uiMask->getSpriteType();
    auto m_spriteTypeCombo = m_uiMaskGroup->createWidget<ComboBox>((int)spriteType);
    m_spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiMask = dynamic_cast<UIMask*>(m_component);
        uiMask->setSpriteType(val);
        dirty();
        });
    m_spriteTypeCombo->setEndOfLine(false);
    m_spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    m_spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    m_spriteTypeCombo->setEndOfLine(true);


    if (spriteType == SpriteType::Sliced) {
        std::array borderLeft = { uiMask->getBorderLeft() };
        m_uiMaskGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiMask = dynamic_cast<UIMask*>(m_component);
            uiMask->setBorderLeft(val[0]);
            });
        std::array borderRight = { uiMask->getBorderRight() };
        m_uiMaskGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiMask = dynamic_cast<UIMask*>(m_component);
            uiMask->setBorderRight(val[0]);
            });
        std::array borderTop = { uiMask->getBorderTop() };
        m_uiMaskGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiMask = dynamic_cast<UIMask*>(m_component);
            uiMask->setBorderTop(val[0]);
            });
        std::array borderBottom = { uiMask->getBorderBottom() };
        m_uiMaskGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiMask = dynamic_cast<UIMask*>(m_component);
            uiMask->setBorderBottom(val[0]);
            });
    }
    else
    {
        auto fillMethod = uiMask->getFillMethod();
        auto m_compComboFillMethod = m_uiMaskGroup->createWidget<ComboBox>((int)fillMethod);
        m_compComboFillMethod->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiMask = dynamic_cast<UIMask*>(m_component);
            uiMask->setFillMethod(val);
            dirty();
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

            auto m_compComboFillOrigin = m_uiMaskGroup->createWidget<ComboBox>((int)uiMask->getFillOrigin());
            m_compComboFillOrigin->getOnDataChangedEvent().addListener([this](auto val) {
                auto uiMask = dynamic_cast<UIMask*>(m_component);
                uiMask->setFillOrigin(val);
                dirty();
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

            std::array fillAmount = { uiMask->getFillAmount() };
            m_uiMaskGroup->createWidget<Drag<float, 1>>("Fill Amount", ImGuiDataType_Float, fillAmount, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto uiMask = dynamic_cast<UIMask*>(m_component);
                uiMask->setFillAmount(val[0]);
                });

            if (fillMethod == FillMethod::Radial90 || fillMethod == FillMethod::Radial180 || fillMethod == FillMethod::Radial360) {
                m_uiMaskGroup->createWidget<CheckBox>("Clockwise", uiMask->getClockwise())->getOnDataChangedEvent().addListener([this](bool val) {
                    auto uiMask = dynamic_cast<UIMask*>(m_component);
                    uiMask->setClockwise(val);
                    });
            }
        }
    }

    auto color = uiMask->getColor();
    m_uiMaskGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiMask = dynamic_cast<UIMask*>(m_component);
        uiMask->setColor(val[0], val[1], val[2], val[3]);
        });
}
NS_IGE_END