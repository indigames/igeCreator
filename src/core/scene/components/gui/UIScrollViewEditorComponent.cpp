#include "core/scene/components/gui/UIScrollViewEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "core/Editor.h"

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

UIScrollViewEditorComponent::~UIScrollViewEditorComponent() {
    m_uiScrollViewGroup = nullptr;
}

void UIScrollViewEditorComponent::onInspectorUpdate() {
    drawUIScrollView();
}

void UIScrollViewEditorComponent::drawUIScrollView() {
    if (m_uiScrollViewGroup == nullptr)
        m_uiScrollViewGroup = m_group->createWidget<Group>("UIScrollViewGroup", false);;
    m_uiScrollViewGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_uiScrollViewGroup->createWidget<CheckBox>("Interactable", comp->getProperty<bool>("interactable", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("interactable", val);
    });

    auto txtPath = m_uiScrollViewGroup->createWidget<TextField>("Background", comp->getProperty<std::string>("path", ""), false, true);
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

    auto spriteType = comp->getProperty<int>("spritetype", -1);
    auto spriteTypeCombo = m_uiScrollViewGroup->createWidget<ComboBox>("Sprite Type", spriteType);
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
    if (spriteType == -1) spriteTypeCombo->addChoice(-1, "");
    spriteTypeCombo->setEndOfLine(true);

    if (spriteType == (int)SpriteType::Sliced) {
        auto border = comp->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
        std::array borderLeft = { border.X() };
        auto b1 = m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f);
        b1->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        b1->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.X(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", border);
        });
        
        std::array borderRight = { border.Y() };
        auto b2 = m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f);
        b2->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        b2->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.Y(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", border);
        });
        
        std::array borderTop = { border.Z() };
        auto b3 = m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f);
        b3->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        b3->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.Z(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", border);
        });
        
        std::array borderBottom = { border.W() };
        auto b4 = m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f);
        b4->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        b4->getOnDataChangedEvent().addListener([this](auto val) {
            auto border = getComponent<CompoundComponent>()->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border.W(val[0]);
            getComponent<CompoundComponent>()->setProperty("border", border);
        });
    }
    else {
        auto fillMethod = comp->getProperty<int>("fillmethod", -1);
        auto fillMethodCombo = m_uiScrollViewGroup->createWidget<ComboBox>("Fill Method", (int)fillMethod);
        fillMethodCombo->getOnDataChangedEvent().addListener([this](auto val) {
            if (val != -1) {
                storeUndo();
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
            auto fillOriginCombo = m_uiScrollViewGroup->createWidget<ComboBox>("Fill Origin", fillOrigin);
            fillOriginCombo->getOnDataChangedEvent().addListener([this](auto val) {
                if (val != -1) {
                    storeUndo();
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
            auto f1 = m_uiScrollViewGroup->createWidget<Drag<float, 1>>("Fill Amount", ImGuiDataType_Float, fillAmount, 0.01f, 0.f, 1.f);
            f1->getOnDataBeginChangedEvent().addListener([this](auto val) {
                storeUndo();
                });
            f1->getOnDataChangedEvent().addListener([this](auto val) {
                getComponent<CompoundComponent>()->setProperty("fillamount", val[0]);
            });

            if (fillMethod == (int)FillMethod::Radial90 || fillMethod == (int)FillMethod::Radial180 || fillMethod == (int)FillMethod::Radial360) {
                m_uiScrollViewGroup->createWidget<CheckBox>("Clockwise", comp->getProperty<bool>("clockwise", false))->getOnDataChangedEvent().addListener([this](bool val) {
                    storeUndo();
                    getComponent<CompoundComponent>()->setProperty("clockwise", val);
                });
            }
        }
    }
    m_uiScrollViewGroup->createWidget<Color>("Color", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
    });

    m_uiScrollViewGroup->createWidget<CheckBox>("Horizontal", comp->getProperty<bool>("enablehorizontal", false))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("enablehorizontal", val);
    });
    m_uiScrollViewGroup->createWidget<CheckBox>("Vertical", comp->getProperty<bool>("enablevertical", false))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("enablevertical", val);
    });

    auto movementType = comp->getProperty<int>("movementtype", -1);
    auto moveTypeCombo = m_uiScrollViewGroup->createWidget<ComboBox>("Move Type", (int)movementType);
    moveTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("movementtype", val);
            setDirty();
        }
    });
    moveTypeCombo->setEndOfLine(false);
    moveTypeCombo->addChoice((int)UIScrollView::MovementType::Elastic, "Elastic");
    moveTypeCombo->addChoice((int)UIScrollView::MovementType::Clamped, "Clamped");
    if(movementType == -1) moveTypeCombo->addChoice(-1, "");
    moveTypeCombo->setEndOfLine(true);

    if (movementType == (int)UIScrollView::MovementType::Elastic) 
    {
        std::array elasticity = { comp->getProperty<float>("elasticity", NAN) };
        auto e1 = m_uiScrollViewGroup->createWidget<Drag<float>>("Elasticity", ImGuiDataType_Float, elasticity, 0.01f, 0.f);
        e1->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        e1->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("elasticity", val[0]);
        });

        auto elasticExtra = comp->getProperty<Vec2>("elasticextra", { NAN, NAN });
        std::array elasticExtraArr = { elasticExtra[0], elasticExtra[1] };
        auto e2 = m_uiScrollViewGroup->createWidget<Drag<float, 2>>("Elastic Extra", ImGuiDataType_Float, elasticExtraArr, 0.1f, 0.f);
        e2->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        e2->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("elasticextra", {val[0], val[1]});
        });
    }

    auto isInertia = comp->getProperty<bool>("inertia", false);
    auto m_inertia = m_uiScrollViewGroup->createWidget<CheckBox>("Inertia", isInertia)->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("inertia", val);
        setDirty();
    });
    if (isInertia) {
        std::array deceleration = { comp->getProperty<float>("decelerationrate", NAN) };
        auto d1 = m_uiScrollViewGroup->createWidget<Drag<float>>("Deceleration Rate", ImGuiDataType_Float, deceleration, 0.01f, 0.f);
        d1->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        d1->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("decelerationrate", val[0]);
        });
    }
}
NS_IGE_END
