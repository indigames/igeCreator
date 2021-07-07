#include "core/scene/components/gui/UIScrollViewEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UIScrollView.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

UIScrollViewEditorComponent::UIScrollViewEditorComponent() {
    m_uiScrollViewGroup = nullptr;
}

UIScrollViewEditorComponent::~UIScrollViewEditorComponent()
{
    m_uiScrollViewGroup = nullptr;
}

void UIScrollViewEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiScrollViewGroup == nullptr) {
        m_uiScrollViewGroup = m_group->createWidget<Group>("UIScrollViewGroup", false);
    }
    drawUIScrollView();

    EditorComponent::redraw();
}

void UIScrollViewEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiScrollViewGroup = m_group->createWidget<Group>("UIScrollViewGroup", false);

    drawUIScrollView();
}

void UIScrollViewEditorComponent::drawUIScrollView()
{
    if (m_uiScrollViewGroup == nullptr)
        return;
    m_uiScrollViewGroup->removeAllWidgets();

    auto uiScrollView = getComponent<UIScrollView>(); 
    if (uiScrollView == nullptr)
        return;

    auto txtPath = m_uiScrollViewGroup->createWidget<TextField>("Path", uiScrollView->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiImage = getComponent<UIImage>(); 
        uiImage->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setPath(txt);
            setDirty();
            });
    }

    m_uiScrollViewGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
        if (files.size() > 0)
        {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setPath(files[0]);
            setDirty();
        }
        });

    auto m_interactable = m_uiScrollViewGroup->createWidget<CheckBox>("Interactable", uiScrollView->isInteractable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiScrollView = getComponent<UIScrollView>(); 
        uiScrollView->setInteractable(val);
        });

    auto spriteType = uiScrollView->getSpriteType();
    auto m_spriteTypeCombo = m_uiScrollViewGroup->createWidget<ComboBox>("", (int)spriteType);
    m_spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollView = getComponent<UIScrollView>(); 
        uiScrollView->setSpriteType(val);
        setDirty();
        });
    m_spriteTypeCombo->setEndOfLine(false);
    m_spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    m_spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    m_spriteTypeCombo->setEndOfLine(true);


    if (spriteType == SpriteType::Sliced) {
        std::array borderLeft = { uiScrollView->getBorderLeft() };
        m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setBorderLeft(val[0]);
            });
        std::array borderRight = { uiScrollView->getBorderRight() };
        m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setBorderRight(val[0]);
            });
        std::array borderTop = { uiScrollView->getBorderTop() };
        m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setBorderTop(val[0]);
            });
        std::array borderBottom = { uiScrollView->getBorderBottom() };
        m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setBorderBottom(val[0]);
            });
    }
    else
    {
        auto fillMethod = uiScrollView->getFillMethod();
        auto m_compComboFillMethod = m_uiScrollViewGroup->createWidget<ComboBox>("", (int)fillMethod);
        m_compComboFillMethod->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setFillMethod(val);
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

            auto m_compComboFillOrigin = m_uiScrollViewGroup->createWidget<ComboBox>("", (int)uiScrollView->getFillOrigin());
            m_compComboFillOrigin->getOnDataChangedEvent().addListener([this](auto val) {
                auto uiScrollView = getComponent<UIScrollView>(); 
                uiScrollView->setFillOrigin(val);
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

            std::array fillAmount = { uiScrollView->getFillAmount() };
            m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Fill Amount", ImGuiDataType_Float, fillAmount, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto uiScrollView = getComponent<UIScrollView>(); 
                uiScrollView->setFillAmount(val[0]);
                });

            if (fillMethod == FillMethod::Radial90 || fillMethod == FillMethod::Radial180 || fillMethod == FillMethod::Radial360) {
                m_uiScrollViewGroup->createWidget<CheckBox>("Clockwise", uiScrollView->getClockwise())->getOnDataChangedEvent().addListener([this](bool val) {
                    auto uiScrollView = getComponent<UIScrollView>(); 
                    uiScrollView->setClockwise(val);
                    });
            }
        }
    }

    auto color = uiScrollView->getColor();
    m_uiScrollViewGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollView = getComponent<UIScrollView>(); 
        uiScrollView->setColor(val[0], val[1], val[2], val[3]);
        });

    auto m_horizontal = m_uiScrollViewGroup->createWidget<CheckBox>("Horizontal", uiScrollView->enableHorizontal())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiScrollView = getComponent<UIScrollView>(); 
        uiScrollView->setEnableHorizontal(val);
        });
    auto m_vertical = m_uiScrollViewGroup->createWidget<CheckBox>("Vertical", uiScrollView->enableVertical())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiScrollView = getComponent<UIScrollView>(); 
        uiScrollView->setEnableVertical(val);
        });

    auto movementType = uiScrollView->getMovementType();
    auto m_movememntTypeCombo = m_uiScrollViewGroup->createWidget<ComboBox>("", (int)movementType);
    m_movememntTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        auto uiScrollView = getComponent<UIScrollView>(); 
        uiScrollView->setMovementType(val);
        setDirty();
        });
    m_movememntTypeCombo->setEndOfLine(false);
    m_movememntTypeCombo->addChoice((int)UIScrollView::MovementType::Elastic, "Elastic");
    m_movememntTypeCombo->addChoice((int)UIScrollView::MovementType::Clamped, "Clamped");
    m_movememntTypeCombo->setEndOfLine(true);

    if (movementType == UIScrollView::MovementType::Elastic) 
    {
        std::array elasticity = { uiScrollView->getElasticity() };
        m_uiScrollViewGroup->createWidget<Drag<float>>("Elasticity", ImGuiDataType_Float, elasticity, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setElasticity(val[0]);
            });

        std::array elasticExtra = { uiScrollView->getElasticExtra()[0], uiScrollView->getElasticExtra()[1] };
        m_uiScrollViewGroup->createWidget<Drag<float, 2>>("Elastic Extra", ImGuiDataType_Float, elasticExtra, 0.1f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            auto elasticExtra = uiScrollView->getElasticExtra();
            elasticExtra[0] = val[0];
            elasticExtra[1] = val[1];
            uiScrollView->setElasticExtra(elasticExtra);
            });
    }

    auto m_inertia = m_uiScrollViewGroup->createWidget<CheckBox>("Inertia", uiScrollView->isInertia())->getOnDataChangedEvent().addListener([this](bool val) {
        auto uiScrollView = getComponent<UIScrollView>(); 
        uiScrollView->setInertia(val);
        setDirty();
        });

    if (uiScrollView->isInertia()) 
    {
        std::array deceleration = { uiScrollView->getDecelerationRate() };
        m_uiScrollViewGroup->createWidget<Drag<float>>("Deceleration Rate", ImGuiDataType_Float, deceleration, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto uiScrollView = getComponent<UIScrollView>(); 
            uiScrollView->setDecelerationRate(val[0]);
            });
    }
}
NS_IGE_END