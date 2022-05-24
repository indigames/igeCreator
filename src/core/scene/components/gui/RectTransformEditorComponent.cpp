#include "core/scene/components/gui/RectTransformEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include "components/gui/RectTransform.h"
#include <core/layout/Group.h>

#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"
#include <core/panels/Inspector.h>

#include "core/task/TaskManager.h"
#include "core/Editor.h"
#include "core/Canvas.h"

NS_IGE_BEGIN

RectTransformEditorComponent::RectTransformEditorComponent() {
    m_anchor_transform_ParentGroup = nullptr;
    m_pivotGroup = nullptr;
    m_anchorGroup = nullptr;
    m_rectGroup = nullptr;
    m_anchorMinMaxGroup = nullptr;

    m_dirtyFlagSupport = 0;
    m_bIsLockTransformUpdate = false;
    m_listenerId = -1;

    m_targetAddedEventId = Editor::getTargetAddedEvent().addListener(std::bind(&RectTransformEditorComponent::onTargetAdded, this, std::placeholders::_1));
    m_targetRemovedEventId = Editor::getTargetRemovedEvent().addListener(std::bind(&RectTransformEditorComponent::onTargetRemoved, this, std::placeholders::_1));
    m_targetClearedEventId = Editor::getTargetClearedEvent().addListener(std::bind(&RectTransformEditorComponent::onTargetCleared, this));

}

RectTransformEditorComponent::~RectTransformEditorComponent() {
    m_anchorGroup = nullptr;
    m_rectGroup = nullptr;
    m_anchorMinMaxGroup = nullptr;
    m_pivotGroup = nullptr;
    m_anchor_transform_ParentGroup = nullptr;

    Editor::getTargetAddedEvent().removeListener(m_targetAddedEventId);
    Editor::getTargetRemovedEvent().removeListener(m_targetRemovedEventId);
    Editor::getTargetClearedEvent().removeListener(m_targetClearedEventId);

    if (!m_lastTarget.expired() && m_listenerId != (uint64_t)-1) {
        m_lastTarget.lock()->getTransformChangedEvent().removeListener(m_listenerId);
        m_listenerId = -1;
    }
    m_lastTarget.reset();
}

bool RectTransformEditorComponent::setComponent(std::shared_ptr<Component> component) {
    bool valid = EditorComponent::setComponent(component);
    if (valid) updateTarget();
    return valid;
}

//! Target events
void RectTransformEditorComponent::onTargetAdded(const std::shared_ptr<SceneObject>& object) {
    updateTarget();
}

void RectTransformEditorComponent::onTargetRemoved(const std::shared_ptr<SceneObject>& object) {
    updateTarget();
}

void RectTransformEditorComponent::updateTarget() {
    if (!m_lastTarget.expired() && m_listenerId != (uint64_t)-1) {
        m_lastTarget.lock()->getTransformChangedEvent().removeListener(m_listenerId);
        m_listenerId = -1;
    }
    auto comp = getComponent<CompoundComponent>();
    if (comp != nullptr) {
        auto comps = comp->getComponents();
        if (comps.size() > 0) {
            m_lastTarget = comps[0]->getOwner()->getSharedPtr();
            if (!m_lastTarget.expired()) {
                m_listenerId = m_lastTarget.lock()->getTransformChangedEvent().addListener(CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            }
        }
    }

}

void RectTransformEditorComponent::onTargetCleared() {
    if (!m_lastTarget.expired() && m_listenerId != (uint64_t)-1) {
        m_lastTarget.lock()->getTransformChangedEvent().removeListener(m_listenerId);
        m_listenerId = -1;
    }
    m_lastTarget.reset();
}

void RectTransformEditorComponent::onInspectorUpdate() {
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    switch (m_dirtyFlagSupport) {
    case 1:
        //! Draw Anchor Btn & Position
        drawAnchor();
        drawRect();
        drawAnchorMinMax();
        break;
    case 2:
        //! Draw Position & Pivot
        drawRect();
        drawPivot();
        break;
    case 3:
        //! Draw Only Position
        drawRect();
        break;
    case 4:
        //! Draw Anchor Btn & Position
        drawAnchor();
        drawRect();
        break;
    default:
        //! Draw All
        drawRectTransform();
        break;
    }
    m_dirtyFlagSupport = 0;
}

void RectTransformEditorComponent::drawRectTransform() {
    if (m_anchor_transform_ParentGroup == nullptr)
        m_anchor_transform_ParentGroup = m_group->createWidget<Group>("TransformAnchorParentGroup", false);
    m_anchor_transform_ParentGroup->removeAllWidgets();

    m_anchor_transform_ParentGroup->createWidget<AnchorPresets>("AnchorPresets")->getOnClickEvent().addListener([this](const auto& widget) {
        storeUndo();
        auto anchorWidget = (AnchorWidget*)widget;
        auto aMin = anchorWidget->getAnchorMin();
        auto aMax = anchorWidget->getAnchorMax();
        getComponent<CompoundComponent>()->setProperty("anchor", {aMin[0], aMin[1], aMax[0], aMax[1]});
        m_dirtyFlagSupport = 1;
        setDirty();
        getComponent<CompoundComponent>()->setDirty();
        if(Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });

    auto anchorColumn = m_anchor_transform_ParentGroup->createWidget<Columns<2>>();
    anchorColumn->setColumnWidth(0, 52.f);
    if (m_anchorGroup) {
        m_anchorGroup->removeAllWidgets();
        m_anchorGroup = nullptr;
    }
    m_anchorGroup = anchorColumn->createWidget<Group>("AnchorGroup", false);

    if (m_rectGroup) {
        m_rectGroup->removeAllWidgets();
        m_rectGroup = nullptr;
    }
    m_rectGroup = anchorColumn->createWidget<Group>("RectGroup", false);

    //! Draw Pivot Button
    drawAnchor();

    //! Draw Position, Size
    drawRect();

    //! Draw Anchor Min, Max
    drawAnchorMinMax();

    //! Draw Pivot, Rotation, Scale
    drawPivot();
}

void RectTransformEditorComponent::drawAnchor() {
    if (m_anchorGroup == nullptr)
        return;
    m_anchorGroup->removeAllWidgets();

    auto anchor = getComponent<CompoundComponent>()->getProperty<Vec4>("anchor", { NAN, NAN, NAN, NAN });
    m_anchorGroup->createWidget<AnchorWidget>(ImVec2(anchor[0], anchor[1]), ImVec2(anchor[2], anchor[3]), false)->getOnClickEvent().addListener([](auto widget) {
        ImGui::OpenPopup("AnchorPresets");
    });
}

void RectTransformEditorComponent::drawRect() {
    if (m_rectGroup == nullptr)
        return;
    m_rectGroup->removeAllWidgets();

    auto anchor = getComponent<CompoundComponent>()->getProperty<Vec4>("anchor", {NAN, NAN, NAN, NAN});
    auto offset = getComponent<CompoundComponent>()->getProperty<Vec4>("offset", { NAN, NAN, NAN, NAN });
    bool showPosX = (anchor[0] == anchor[2]);
    bool showPosY = (anchor[1] == anchor[3]);

    auto anchoredPos = getComponent<CompoundComponent>()->getProperty<Vec2>("anchorposition", { NAN, NAN });
    auto size = getComponent<CompoundComponent>()->getProperty<Vec2>("size", { NAN, NAN });
    float a = showPosX ? anchoredPos.X() : offset[0];
    float b = showPosY ? anchoredPos.Y() : offset[3];
    float c = showPosX ? size.X() : offset[2];
    float d = showPosY ? size.Y() : offset[1];

    std::array A = { a };
    auto anchorGroupColums = m_rectGroup->createWidget<Columns<3>>(-1.f, true, 52.f);
    auto anchor1 = anchorGroupColums->createWidget<Drag<float>>(showPosX ? "X" : "L", ImGuiDataType_Float, A, 1.f);
    anchor1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    anchor1->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto comp = getComponent<CompoundComponent>();
        auto anchor = comp->getProperty<Vec4>("anchor", { NAN, NAN, NAN, NAN });
        bool showPosX = (anchor[0] == anchor[2]);
        if (showPosX) {
            auto anchoredPos = comp->getProperty<Vec2>("anchorposition", { NAN, NAN });
            anchoredPos.X(val[0]);
            comp->setProperty("anchorposition", anchoredPos);
        } else {
            auto offset = comp->getProperty<Vec4>("offset", { NAN, NAN });
            offset[0] = val[0];
            comp->setProperty("offset", offset);
        }
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });

    std::array B = { b };
    auto anchor2 = anchorGroupColums->createWidget<Drag<float>>(showPosY ? "Y" : "T", ImGuiDataType_Float, B, 1.f);
    anchor2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    anchor2->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto comp = getComponent<CompoundComponent>();
        auto anchor = comp->getProperty<Vec4>("anchor", { NAN, NAN, NAN, NAN });
        bool showPosY = (anchor[1] == anchor[3]);
        if (showPosY) {
            auto anchoredPos = comp->getProperty<Vec2>("anchorposition", { NAN, NAN });
            anchoredPos.Y(val[0]);
            comp->setProperty("anchorposition", anchoredPos);
        } else {
            auto offset = comp->getProperty<Vec4>("offset", { NAN, NAN });
            offset[3] = val[0];
            comp->setProperty("offset", offset);
        }
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });

    std::array posZ = { getComponent<CompoundComponent>()->getProperty<Vec3>("pos", { NAN, NAN, NAN }).Z() };
    auto anchor3 = anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 0.01f);
    anchor3->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    anchor3->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto comp = getComponent<CompoundComponent>();
        auto position = comp->getProperty<Vec3>("pos", { NAN, NAN, NAN });
        position.Z(val[0]);
        comp->setProperty("pos", position);
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });

    std::array C = { c };
    auto anchor4 = anchorGroupColums->createWidget<Drag<float>>(showPosX ? "W" : "R", ImGuiDataType_Float, C, 1.f);
    anchor4->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    anchor4->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto comp = getComponent<CompoundComponent>();
        auto anchor = comp->getProperty<Vec4>("anchor", { NAN, NAN, NAN, NAN });
        bool showPosX = (anchor[0] == anchor[2]);
        if (showPosX) {
            auto size = comp->getProperty<Vec2>("size", { NAN, NAN });
            size.X(val[0]);
            comp->setProperty("size", size);
        } else {
            auto offset = comp->getProperty<Vec4>("offset", { NAN, NAN });
            offset[2] = val[0];
            comp->setProperty("offset", offset);
        }
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });

    std::array D = { d };
    auto anchor5 = anchorGroupColums->createWidget<Drag<float>>(showPosY ? "H" : "B", ImGuiDataType_Float, D, 1.f);
    anchor5->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    anchor5->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto comp = getComponent<CompoundComponent>();
        auto anchor = comp->getProperty<Vec4>("anchor", { NAN, NAN, NAN, NAN });
        bool showPosY = (anchor[1] == anchor[3]);
        if (showPosY) {
            auto size = comp->getProperty<Vec2>("size", { NAN, NAN });
            size.Y(val[0]);
            comp->setProperty("size", size);
        } else {
            auto offset = comp->getProperty<Vec4>("offset", { NAN, NAN });
            offset[1] = val[0];
            comp->setProperty("offset", offset);
        }
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });
}

void RectTransformEditorComponent::drawAnchorMinMax() {
   if (m_anchorMinMaxGroup == nullptr)
        m_anchorMinMaxGroup = m_group->createWidget<Group>("AnchorMinMaxGroup", false);
    m_anchorMinMaxGroup->removeAllWidgets();

    auto anchor = getComponent<CompoundComponent>()->getProperty<Vec4>("anchor", { NAN, NAN, NAN, NAN });
    std::array anchorMin = { anchor[0], anchor[1] };
    auto anchor1 = m_anchorMinMaxGroup->createWidget<Drag<float, 2>>("Anchor Min", ImGuiDataType_Float, anchorMin, 0.01f, 0.f, 1.f);
    anchor1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    anchor1->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto anchor = getComponent<CompoundComponent>()->getProperty<Vec4>("anchor", { NAN, NAN, NAN, NAN });
        anchor[0] = val[0];
        anchor[1] = val[1];
        getComponent<CompoundComponent>()->setProperty("anchor", anchor);
        m_dirtyFlagSupport = 4;
        setDirty();
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });

    std::array anchorMax = { anchor[2], anchor[3] };
    auto anchor2 = m_anchorMinMaxGroup->createWidget<Drag<float, 2>>("Anchor Max", ImGuiDataType_Float, anchorMax, 0.01f, 0.f, 1.f);
    anchor2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    anchor2->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto anchor = getComponent<CompoundComponent>()->getProperty<Vec4>("anchor", { NAN, NAN, NAN, NAN });
        anchor[2] = val[0];
        anchor[3] = val[1];
        getComponent<CompoundComponent>()->setProperty("anchor", anchor);
        m_dirtyFlagSupport = 4;
        setDirty();
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });
}

void RectTransformEditorComponent::drawPivot() {
    if (m_pivotGroup == nullptr)
        m_pivotGroup = m_group->createWidget<Group>("PivotGroup", false);;
    m_pivotGroup->removeAllWidgets();

    auto pivot = getComponent<CompoundComponent>()->getProperty<Vec2>("pivot", { NAN, NAN });
    std::array pivotArr = { pivot.X(), pivot.Y() };
    auto p1 = m_pivotGroup->createWidget<Drag<float, 2>>("Pivot", ImGuiDataType_Float, pivotArr, 0.01f, 0.f, 1.f);
    p1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    p1->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        getComponent<CompoundComponent>()->setProperty("pivot", { val[0], val[1] });
        m_dirtyFlagSupport = 3;
        setDirty();
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });

    auto rotE = getComponent<CompoundComponent>()->getProperty<Vec3>("rot", { NAN, NAN, NAN });
    std::array rot = { RADIANS_TO_DEGREES(rotE.X()), RADIANS_TO_DEGREES(rotE.Y()), RADIANS_TO_DEGREES(rotE.Z()) };
    auto r1 = m_pivotGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot);
    r1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    r1->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        getComponent<CompoundComponent>()->setProperty("rot", Vec3(DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2])));
        m_dirtyFlagSupport = 3;
        setDirty();
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });

    auto scale = getComponent<CompoundComponent>()->getProperty<Vec3>("scale", { NAN, NAN, NAN });
    std::array scaleArr = { scale.X(), scale.Y(), scale.Z() };
    auto s1 = m_pivotGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scaleArr);
    s1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    s1->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        getComponent<CompoundComponent>()->setProperty("scale", { val[0], val[1], val[2] });
        m_dirtyFlagSupport = 3;
        setDirty();
        getComponent<CompoundComponent>()->setDirty();
        if (Editor::getCanvas()->getEditorScene()->getGizmo())
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
    });
}

void RectTransformEditorComponent::onTransformChanged(SceneObject& sceneObject)
{
    // Just redraw the transform in Inspector
    if (!m_component.expired()) {
        if (m_dirtyFlagSupport != 1 && m_dirtyFlagSupport != 4) {
            m_dirtyFlagSupport = 2;
            if (getComponent<CompoundComponent>()) {
                getComponent<CompoundComponent>()->setDirty();
            }
            setDirty();
        }
    }
    if (Editor::getCanvas()->getEditorScene()->getGizmo())
        Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
}

NS_IGE_END