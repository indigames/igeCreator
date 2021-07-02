#include "core/scene/components/gui/RectTransformEditorComponent.h"

#include "components/gui/RectTransform.h"
#include <core/layout/Group.h>

#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"
#include <core/panels/Inspector.h>

#include "core/task/TaskManager.h"
#include "core/Editor.h"
#include "core/Canvas.h"

NS_IGE_BEGIN

RectTransformEditorComponent::RectTransformEditorComponent() 
{
    m_anchor_transform_ParentGroup = nullptr;
    m_pivotGroup = nullptr;
    m_anchorGroup = nullptr;
    m_rectGroup = nullptr;
    m_anchorMinMaxGroup = nullptr;

    m_dirtyFlagSupport = 0;
    m_bIsLockTransformUpdate = false;
    m_listenerId = -1;
}

RectTransformEditorComponent::~RectTransformEditorComponent()
{
    m_anchorGroup = nullptr;
    m_rectGroup = nullptr;
    m_anchorMinMaxGroup = nullptr;
    m_pivotGroup = nullptr;
    m_anchor_transform_ParentGroup = nullptr;
}

bool RectTransformEditorComponent::setComponent(std::shared_ptr<Component> component)
{
    if (m_component != component)
    {
        if (m_component)
        {
            auto oldTarget = m_component->getOwner();
            if (oldTarget)
            {
                if (m_listenerId != (uint64_t)-1)
                {
                    oldTarget->getTransformChangedEvent().removeListener(m_listenerId);
                    m_listenerId = -1;
                }
            }
        }
        if (component)
        {
            auto newTarget = component->getOwner();
            if (newTarget != nullptr)
            {
                m_listenerId = newTarget->getTransformChangedEvent().addListener(CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            }
        }
    }
    return EditorComponent::setComponent(component);
}

void RectTransformEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_pivotGroup == nullptr) {
        m_pivotGroup = m_group->createWidget<Group>("PivotGroup", false);
    }

    if (m_anchorMinMaxGroup == nullptr) {
        m_anchorMinMaxGroup = m_group->createWidget<Group>("RectTransformAnchorMinMaxGroup", false);
    }

    if (m_anchor_transform_ParentGroup == nullptr) {
        m_anchor_transform_ParentGroup = m_group->createWidget<Group>("TransformAnchorParentGroup", false);
    }

    switch (m_dirtyFlagSupport) {
    case 0:
    default:
        //! Draw All
        drawRectTransform();
        break;
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
    }
    
    EditorComponent::redraw();
    m_dirtyFlagSupport = 0;
}

void RectTransformEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    if (m_anchor_transform_ParentGroup == nullptr) 
        m_anchor_transform_ParentGroup = m_group->createWidget<Group>("TransformAnchorParentGroup", false);

    if (m_anchorMinMaxGroup == nullptr) 
        m_anchorMinMaxGroup = m_group->createWidget<Group>("AnchorMinMaxGroup", false);

    if (m_pivotGroup == nullptr) 
        m_pivotGroup = m_group->createWidget<Group>("PivotGroup", false);
    

    drawRectTransform();
}

void RectTransformEditorComponent::drawRectTransform() {
    if (m_pivotGroup == nullptr || m_anchor_transform_ParentGroup == nullptr || m_anchorMinMaxGroup == nullptr) return;

    if (m_anchorGroup) {
        m_anchorGroup->removeAllWidgets();
        m_anchorGroup = nullptr;
    }
    
    if (m_rectGroup) {
        m_rectGroup->removeAllWidgets();
        m_rectGroup = nullptr;
    }

    m_anchor_transform_ParentGroup->removeAllWidgets();
    m_anchor_transform_ParentGroup->createWidget<AnchorPresets>("AnchorPresets")->getOnClickEvent().addListener([this](const auto& widget) {
        auto rectTransform = getComponent<RectTransform>();
        auto anchor = rectTransform->getAnchor();
        auto anchorWidget = (AnchorWidget*)widget;
        anchor[0] = anchorWidget->getAnchorMin().x;
        anchor[1] = anchorWidget->getAnchorMin().y;
        anchor[2] = anchorWidget->getAnchorMax().x;
        anchor[3] = anchorWidget->getAnchorMax().y;
        rectTransform->setAnchor(anchor);
        m_dirtyFlagSupport = 1;
        dirty();
    });

    auto anchorColumn = m_anchor_transform_ParentGroup->createWidget<Columns<2>>();
    anchorColumn->setColumnWidth(0, 52.f);
    m_anchorGroup = anchorColumn->createWidget<Group>("AnchorGroup", false);
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
    auto rectTransform = getComponent<RectTransform>();
    if (rectTransform == nullptr)
        return;

    if (m_anchorGroup == nullptr)
        return;

    m_anchorGroup->removeAllWidgets();

    auto anchor = rectTransform->getAnchor();
    auto offset = rectTransform->getOffset();

    m_anchorGroup->createWidget<AnchorWidget>(ImVec2(anchor[0], anchor[1]), ImVec2(anchor[2], anchor[3]), false)->getOnClickEvent().addListener([](auto widget) {
        ImGui::OpenPopup("AnchorPresets");
    });
}

void RectTransformEditorComponent::drawRect() {
    auto rectTransform = getComponent<RectTransform>();
    if (rectTransform == nullptr)
        return;

    if (m_rectGroup == nullptr)
        return;

    m_rectGroup->removeAllWidgets();
    auto anchor = rectTransform->getAnchor();
    auto offset = rectTransform->getOffset();

    auto anchorGroupColums = m_rectGroup->createWidget<Columns<3>>(-1.f, true, 52.f);

    bool showPosX = (anchor[0] == anchor[2]);
    bool showPosY = (anchor[1] == anchor[3]);

    //! Pos X or Left
    float a = showPosX ? rectTransform->getAnchoredPosition().X() : offset[0];
    //! Pos Y or Top
    float b = showPosY ? rectTransform->getAnchoredPosition().Y() : offset[3];
    //! Width or Right 
    float c = showPosX ? rectTransform->getSize().X() : offset[2];
    //! Height or Bottom
    float d = showPosY ? rectTransform->getSize().Y() : offset[1];

    std::array A = { a };
    anchorGroupColums->createWidget<Drag<float>>(showPosX ? "X" : "L", ImGuiDataType_Float, A, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        auto anchor = rectTransform->getAnchor();
        bool showPosX = (anchor[0] == anchor[2]);
        if (showPosX)
        {
            auto position = rectTransform->getAnchoredPosition();
            position.X(val[0]);
            rectTransform->setAnchoredPosition(position);
        }
        else 
        {
            auto offset = rectTransform->getOffset();
            offset[0] = val[0];
            rectTransform->setOffset(offset);
        }
        rectTransform->onUpdate(0.f);
    });

    std::array B = { b };
    anchorGroupColums->createWidget<Drag<float>>(showPosY ? "Y" : "T", ImGuiDataType_Float, B, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        auto anchor = rectTransform->getAnchor();
        bool showPosY = (anchor[1] == anchor[3]);
        if (showPosY) 
        {
            auto position = rectTransform->getAnchoredPosition();
            position.Y(val[0]);
            rectTransform->setAnchoredPosition(position);
        }
        else
        {
            auto offset = rectTransform->getOffset();
            offset[3] = val[0];
            rectTransform->setOffset(offset);
        }
        rectTransform->onUpdate(0.f);
    });

    std::array posZ = { rectTransform->getPosition().Z() };
    anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        auto position = rectTransform->getPosition();
        position.Z(val[0]);
        rectTransform->setPosition(position);
        rectTransform->onUpdate(0.f);
    });

    std::array C = { c };
    anchorGroupColums->createWidget<Drag<float>>(showPosX ? "W" : "R", ImGuiDataType_Float, C, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        auto anchor = rectTransform->getAnchor();
        bool showPosX = (anchor[0] == anchor[2]);
        if (showPosX)
        {
            auto size = rectTransform->getSize();
            size.X(val[0]);
            rectTransform->setSize(size);
        }
        else
        {
            auto offset = rectTransform->getOffset();
            offset[2] = val[0];
            rectTransform->setOffset(offset);
        }
        rectTransform->onUpdate(0.f);
    });

    std::array D = { d };
    anchorGroupColums->createWidget<Drag<float>>(showPosY ? "H" : "B", ImGuiDataType_Float, D, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        auto anchor = rectTransform->getAnchor();
        bool showPosY = (anchor[1] == anchor[3]);
        if (showPosY)
        {
            auto size = rectTransform->getSize();
            size.Y(val[0]);
            rectTransform->setSize(size);
        }
        else
        {
            auto offset = rectTransform->getOffset();
            offset[1] = val[0];
            rectTransform->setOffset(offset);
        }
        rectTransform->onUpdate(0.f);
    });

}

void RectTransformEditorComponent::drawAnchorMinMax()
{
    auto rectTransform = getComponent<RectTransform>();
    if (rectTransform == nullptr)
        return;

    if (m_anchorMinMaxGroup == nullptr)
        return;
    m_anchorMinMaxGroup->removeAllWidgets();
    auto anchor = rectTransform->getAnchor();

    std::array anchorMin = { anchor[0], anchor[1] };
    m_anchorMinMaxGroup->createWidget<Drag<float, 2>>("Anchor Min", ImGuiDataType_Float, anchorMin, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        auto anchor = rectTransform->getAnchor();
        anchor[0] = val[0];
        anchor[1] = val[1];
        rectTransform->setAnchor(anchor);
        m_dirtyFlagSupport = 4;
        dirty();
    });

    std::array anchorMax = { anchor[2], anchor[3] };
    m_anchorMinMaxGroup->createWidget<Drag<float, 2>>("Anchor Max", ImGuiDataType_Float, anchorMax, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        auto anchor = rectTransform->getAnchor();
        anchor[2] = val[0];
        anchor[3] = val[1];
        rectTransform->setAnchor(anchor);
        m_dirtyFlagSupport = 4;
        dirty();
    });
}

void RectTransformEditorComponent::drawPivot() {
    if (m_pivotGroup == nullptr) return;
    m_pivotGroup->removeAllWidgets();

    auto rectTransform = getComponent<RectTransform>();
    if (rectTransform == nullptr)
        return;

    std::array pivot = { rectTransform->getPivot().X(), rectTransform->getPivot().Y() };
    m_pivotGroup->createWidget<Drag<float, 2>>("Pivot", ImGuiDataType_Float, pivot, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        rectTransform->setPivot({ val[0], val[1] });
        rectTransform->onUpdate(0.f);
        m_dirtyFlagSupport = 3;
        dirty();
    });

    Vec3 euler;
    vmath_quatToEuler(rectTransform->getRotation().P(), euler.P());
    std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
    m_pivotGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        Quat quat;
        float rad[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
        vmath_eulerToQuat(rad, quat.P());
        rectTransform->setRotation(quat);
        rectTransform->onUpdate(0.f);
        m_dirtyFlagSupport = 3;
        dirty();
    });

    std::array scale = { rectTransform->getScale().X(), rectTransform->getScale().Y(), rectTransform->getScale().Z() };
    m_pivotGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = getComponent<RectTransform>();
        rectTransform->setScale({ val[0], val[1], val[2] });
        rectTransform->onUpdate(0.f);
        m_dirtyFlagSupport = 3;
        dirty();
    });
}

void RectTransformEditorComponent::onTransformChanged(SceneObject& sceneObject)
{
    // Just redraw the transform in Inspector
    TaskManager::getInstance()->addTask([this]() {
        if (m_component != nullptr)
        {
            if (m_dirtyFlagSupport != 1 && m_dirtyFlagSupport != 4) {
                m_dirtyFlagSupport = 2;
                dirty();
            }
        }
    });
}

NS_IGE_END