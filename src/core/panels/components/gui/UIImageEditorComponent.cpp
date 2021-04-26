#include "core/panels/components/gui/UIImageEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UIImage.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

UIImageEditorComponent::UIImageEditorComponent() {
    m_uiImageGroup = nullptr;
}

UIImageEditorComponent::~UIImageEditorComponent()
{
    if (m_uiImageGroup) {
        m_uiImageGroup->removeAllWidgets();
        m_uiImageGroup->removeAllPlugins();
    }
    m_uiImageGroup = nullptr;
}

bool UIImageEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<UIImage*>(comp);
}

void UIImageEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiImageGroup == nullptr) {
        m_uiImageGroup = m_group->createWidget<Group>("UIImageGroup", false);
    }
    drawUIImage();

    EditorComponent::redraw();
}

void UIImageEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiImageGroup = m_group->createWidget<Group>("UIImageGroup", false);

    drawUIImage();
}

void UIImageEditorComponent::drawUIImage()
{
    if (m_uiImageGroup == nullptr)
        return;
    m_uiImageGroup->removeAllWidgets();

    auto uiImage = dynamic_cast<UIImage*>(m_component);
    if (uiImage == nullptr)
        return;

    auto txtPath = m_uiImageGroup->createWidget<TextField>("Path", uiImage->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiImage = dynamic_cast<UIImage*>(m_component);
        uiImage->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto uiImage = dynamic_cast<UIImage*>(m_component);
            uiImage->setPath(txt);
            dirty();
            });
    }

    m_uiImageGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
        if (files.size() > 0)
        {
            auto uiImage = m_targetObject->getComponent<UIImage>();
            uiImage->setPath(files[0]);
            dirty();
        }
        });

    auto m_interactable = m_uiImageGroup->createWidget<CheckBox>("Interactable", uiImage->isInteractable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiImage = dynamic_cast<UIImage*>(m_component);
        uiImage->setInteractable(val);
        });

    auto spriteType = uiImage->getSpriteType();
    auto m_spriteTypeCombo = m_uiImageGroup->createWidget<ComboBox>((int)spriteType);
    m_spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiImage = dynamic_cast<UIImage*>(m_component);
        uiImage->setSpriteType(val);
        dirty();
        });
    m_spriteTypeCombo->setEndOfLine(false);
    m_spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    m_spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    m_spriteTypeCombo->setEndOfLine(true);


    if (spriteType == SpriteType::Sliced) {
        std::array borderLeft = { uiImage->getBorderLeft() };
        m_uiImageGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiImage = dynamic_cast<UIImage*>(m_component);
            uiImage->setBorderLeft(val[0]);
            });
        std::array borderRight = { uiImage->getBorderRight() };
        m_uiImageGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiImage = dynamic_cast<UIImage*>(m_component);
            uiImage->setBorderRight(val[0]);
            });
        std::array borderTop = { uiImage->getBorderTop() };
        m_uiImageGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiImage = dynamic_cast<UIImage*>(m_component);
            uiImage->setBorderTop(val[0]);
            });
        std::array borderBottom = { uiImage->getBorderBottom() };
        m_uiImageGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiImage = dynamic_cast<UIImage*>(m_component);
            uiImage->setBorderBottom(val[0]);
            });
    }
    else
    {
        auto fillMethod = uiImage->getFillMethod();
        auto m_compComboFillMethod = m_uiImageGroup->createWidget<ComboBox>((int)fillMethod);
        m_compComboFillMethod->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiImage = dynamic_cast<UIImage*>(m_component);
            uiImage->setFillMethod(val);
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

            auto m_compComboFillOrigin = m_uiImageGroup->createWidget<ComboBox>((int)uiImage->getFillOrigin());
            m_compComboFillOrigin->getOnDataChangedEvent().addListener([this](auto val) {
                auto uiImage = dynamic_cast<UIImage*>(m_component);
                uiImage->setFillOrigin(val);
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

            std::array fillAmount = { uiImage->getFillAmount() };
            m_uiImageGroup->createWidget<Drag<float, 1>>("Fill Amount", ImGuiDataType_Float, fillAmount, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto uiImage = dynamic_cast<UIImage*>(m_component);
                uiImage->setFillAmount(val[0]);
                });

            if (fillMethod == FillMethod::Radial90 || fillMethod == FillMethod::Radial180 || fillMethod == FillMethod::Radial360) {
                m_uiImageGroup->createWidget<CheckBox>("Clockwise", uiImage->getClockwise())->getOnDataChangedEvent().addListener([this](bool val) {
                    auto uiImage = dynamic_cast<UIImage*>(m_component);
                    uiImage->setClockwise(val);
                    });
            }
        }
    }
    /*std::array alpha = { uiImage->getAlpha() };
    m_uiImageGroup->createWidget<Drag<float, 1>>("Alpha", ImGuiDataType_Float, alpha, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiImage = dynamic_cast<UIImage*>(m_component);
        uiImage->setAlpha(val[0]);
        });*/

    auto color = uiImage->getColor();
    m_uiImageGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiImage = dynamic_cast<UIImage*>(m_component);
        uiImage->setColor(val[0], val[1], val[2], val[3]);
        });
}
NS_IGE_END