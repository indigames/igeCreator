#include "core/panels/components/gui/RectTransformEditorComponent.h"

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
    m_rectTransformGroup = nullptr;
    m_rectTransformAnchorGroup = nullptr;
    m_anchorGroup = nullptr;
    m_anchorTransformGroup = nullptr;
    m_dirtyFlagSupport = 0;
}

RectTransformEditorComponent::~RectTransformEditorComponent()
{
    if (m_anchorGroup) {
        m_anchorGroup->removeAllWidgets();
        m_anchorGroup->removeAllPlugins();
    }
    m_anchorGroup = nullptr;

    if (m_anchorTransformGroup) {
        m_anchorTransformGroup->removeAllWidgets();
        m_anchorTransformGroup->removeAllPlugins();
    }
    m_anchorTransformGroup = nullptr;

    if (m_rectTransformGroup) {
        m_rectTransformGroup->removeAllWidgets();
        m_rectTransformGroup->removeAllPlugins();
    }
    m_rectTransformGroup = nullptr;

    if (m_rectTransformAnchorGroup) {
        m_rectTransformAnchorGroup->removeAllWidgets();
        m_rectTransformAnchorGroup->removeAllPlugins();
    }
    m_rectTransformAnchorGroup = nullptr;
}

void RectTransformEditorComponent::setTargetObject(const std::shared_ptr<SceneObject>& obj)
{
    if (m_targetObject != obj)
    {
        if (m_targetObject)
        {
            auto listenerId = Editor::getInstance()->getCanvas()->getInspector()->getTransformListenerId();
            if (listenerId != (uint64_t)-1)
                m_targetObject->getTransformChangedEvent().removeListener(listenerId);
        }
        m_targetObject = obj;

        if (m_targetObject != nullptr)
        {
            auto listenerId = m_targetObject->getTransformChangedEvent().addListener(CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            Editor::getInstance()->getCanvas()->getInspector()->setTransformListenerId(listenerId);
        }
    }
}

bool RectTransformEditorComponent::isSafe(Component* comp)
{
	return dynamic_cast<RectTransform*>(comp);
}

void RectTransformEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_rectTransformGroup == nullptr) {
        m_rectTransformGroup = m_group->createWidget<Group>("RectTransformGroup", false);
    }

    if (m_rectTransformAnchorGroup == nullptr) {
        m_rectTransformAnchorGroup = m_group->createWidget<Group>("RectTransformGroup", false);
    }

    switch (m_dirtyFlagSupport) {
    case 0:
    default:
        drawRectTransform();
        break;
    case 1:
        drawAnchor();
        drawRectTransformAnchor();
        break;
    case 2:
        drawRectTransformAnchor();
        drawTransform();
        break;
    case 3:
        drawRectTransformAnchor();
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

    if (m_rectTransformAnchorGroup == nullptr) {
        m_rectTransformAnchorGroup = m_group->createWidget<Group>("RectTransformAnchorGroup", false);
    }

    if (m_rectTransformGroup == nullptr) {
        m_rectTransformGroup = m_group->createWidget<Group>("RectTransformGroup", false);
    }

    

    drawRectTransform();
}

void RectTransformEditorComponent::drawRectTransform() {
    if (m_rectTransformGroup == nullptr || m_rectTransformAnchorGroup == nullptr) return;

    if (m_anchorGroup) {
        m_anchorGroup->removeAllWidgets();
        m_anchorGroup->removeAllPlugins();
    }
    m_anchorGroup = nullptr;

    if (m_anchorTransformGroup) {
        m_anchorTransformGroup->removeAllWidgets();
        m_anchorTransformGroup->removeAllPlugins();
    }
    m_anchorTransformGroup = nullptr;


    m_rectTransformAnchorGroup->removeAllPlugins();
    m_rectTransformAnchorGroup->removeAllWidgets();

    m_rectTransformAnchorGroup->createWidget<AnchorPresets>("AnchorPresets")->getOnClickEvent().addListener([this](const auto& widget) {
        auto rectTransform = dynamic_cast<RectTransform*>(m_component);
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
    
    auto anchorColumn = m_rectTransformAnchorGroup->createWidget<Columns<2>>();
    anchorColumn->setColumnWidth(0, 52.f);
    m_anchorGroup = anchorColumn->createWidget<Group>("AnchorGroup", false);
    m_anchorTransformGroup = anchorColumn->createWidget<Group>("AnchorTransformGroup", false);

    //! Draw Pivot Button
    drawAnchor();
    //! Draw Position, Size
    drawRectTransformAnchor();
    //! Draw Pivot, Rotation, Scale
    drawTransform();
}

void RectTransformEditorComponent::drawAnchor() {
    auto rectTransform = dynamic_cast<RectTransform*>(m_component);
    if (rectTransform == nullptr)
        return;

    if (m_anchorGroup == nullptr)
        return;

    m_anchorGroup->removeAllWidgets();
    m_anchorGroup->removeAllPlugins();

    auto anchor = rectTransform->getAnchor();
    auto offset = rectTransform->getOffset();

    m_anchorGroup->createWidget<AnchorWidget>(ImVec2(anchor[0], anchor[1]), ImVec2(anchor[2], anchor[3]), false)->getOnClickEvent().addListener([](auto widget) {
        ImGui::OpenPopup("AnchorPresets");
        });
}

void RectTransformEditorComponent::drawRectTransformAnchor() {
    auto rectTransform = dynamic_cast<RectTransform*>(m_component);
    if (rectTransform == nullptr)
        return;

    if (m_anchorTransformGroup == nullptr)
        return;

    m_anchorTransformGroup->removeAllWidgets();
    m_anchorTransformGroup->removeAllPlugins();

    auto anchor = rectTransform->getAnchor();
    auto offset = rectTransform->getOffset();

    auto anchorGroupColums = m_anchorTransformGroup->createWidget<Columns<3>>(-1.f, true, 52.f);

    if ((anchor[0] == 0.f && anchor[2] == 1.f) && (anchor[1] == 0.f && anchor[3] == 1.f))
    {
        std::array left = { offset[0] };
        anchorGroupColums->createWidget<Drag<float>>("L", ImGuiDataType_Float, left, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto offset = rectTransform->getOffset();
            offset[0] = val[0];
            rectTransform->setOffset(offset);
            rectTransform->onUpdate(0.f);
            });

        std::array top = { offset[1] };
        anchorGroupColums->createWidget<Drag<float>>("T", ImGuiDataType_Float, top, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto offset = rectTransform->getOffset();
            offset[1] = val[0];
            rectTransform->setOffset(offset);
            rectTransform->onUpdate(0.f);
            });

        std::array posZ = { rectTransform->getPosition().Z() };
        anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto position = rectTransform->getPosition();
            position.Z(val[0]);
            rectTransform->setPosition(position);
            rectTransform->onUpdate(0.f);
            });

        std::array right = { offset[2] };
        anchorGroupColums->createWidget<Drag<float>>("R", ImGuiDataType_Float, right, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto offset = rectTransform->getOffset();
            offset[2] = val[0];
            rectTransform->setOffset(offset);
            rectTransform->onUpdate(0.f);
            });

        std::array bottom = { offset[3] };
        anchorGroupColums->createWidget<Drag<float>>("B", ImGuiDataType_Float, bottom, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto offset = rectTransform->getOffset();
            offset[3] = val[0];
            rectTransform->setOffset(offset);
            rectTransform->onUpdate(0.f);
            });
    }
    else if (anchor[0] == 0.f && anchor[2] == 1.f)
    {
        std::array left = { offset[0] };
        anchorGroupColums->createWidget<Drag<float>>("L", ImGuiDataType_Float, left, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto offset = rectTransform->getOffset();
            offset[0] = val[0];
            rectTransform->setOffset(offset);
            rectTransform->onUpdate(0.f);
            });
        std::array posY = { rectTransform->getPosition().Y() };
        anchorGroupColums->createWidget<Drag<float>>("Y", ImGuiDataType_Float, posY, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto position = rectTransform->getPosition();
            position.Y(val[0]);
            rectTransform->setPosition(position);
            rectTransform->onUpdate(0.f);
            });
        std::array posZ = { rectTransform->getPosition().Z() };
        anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto position = rectTransform->getPosition();
            position.Z(val[0]);
            rectTransform->setPosition(position);
            rectTransform->onUpdate(0.f);
            });

        std::array right = { offset[2] };
        anchorGroupColums->createWidget<Drag<float>>("R", ImGuiDataType_Float, right, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto offset = rectTransform->getOffset();
            offset[2] = val[0];
            rectTransform->setOffset(offset);
            rectTransform->onUpdate(0.f);
            });

        std::array height = { rectTransform->getSize().Y() };
        anchorGroupColums->createWidget<Drag<float>>("H", ImGuiDataType_Float, height, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto size = rectTransform->getSize();
            size.Y(val[0]);
            rectTransform->setSize(size);
            rectTransform->onUpdate(0.f);
            });
    }
    else if (anchor[1] == 0.f && anchor[3] == 1.f)
    {
        std::array posX = { rectTransform->getPosition().X() };
        anchorGroupColums->createWidget<Drag<float>>("X", ImGuiDataType_Float, posX, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto position = rectTransform->getPosition();
            position.X(val[0]);
            rectTransform->setPosition(position);
            rectTransform->onUpdate(0.f);
            });

        std::array top = { offset[1] };
        anchorGroupColums->createWidget<Drag<float>>("T", ImGuiDataType_Float, top, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto offset = rectTransform->getOffset();
            offset[1] = val[0];
            rectTransform->setOffset(offset);
            rectTransform->onUpdate(0.f);
            });

        std::array posZ = { rectTransform->getPosition().Z() };
        anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto position = rectTransform->getPosition();
            position.Z(val[0]);
            rectTransform->setPosition(position);
            rectTransform->onUpdate(0.f);
            });

        std::array width = { rectTransform->getSize().X() };
        anchorGroupColums->createWidget<Drag<float>>("W", ImGuiDataType_Float, width, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto size = rectTransform->getSize();
            size.X(val[0]);
            rectTransform->setSize(size);
            rectTransform->onUpdate(0.f);
            });

        std::array bottom = { offset[3] };
        anchorGroupColums->createWidget<Drag<float>>("B", ImGuiDataType_Float, bottom, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto offset = rectTransform->getOffset();
            offset[3] = val[0];
            rectTransform->setOffset(offset);
            rectTransform->onUpdate(0.f);
            });
    }
    else
    {
        std::array posX = { rectTransform->getPosition().X() };
        anchorGroupColums->createWidget<Drag<float>>("X", ImGuiDataType_Float, posX, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto position = rectTransform->getPosition();
            position.X(val[0]);
            rectTransform->setPosition(position);
            rectTransform->onUpdate(0.f);
            });

        std::array posY = { rectTransform->getPosition().Y() };
        anchorGroupColums->createWidget<Drag<float>>("Y", ImGuiDataType_Float, posY, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto position = rectTransform->getPosition();
            position.Y(val[0]);
            rectTransform->setPosition(position);
            rectTransform->onUpdate(0.f);
            });

        std::array posZ = { rectTransform->getPosition().Z() };
        anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto position = rectTransform->getPosition();
            position.Z(val[0]);
            rectTransform->setPosition(position);
            rectTransform->onUpdate(0.f);
            });

        std::array width = { rectTransform->getSize().X() };
        anchorGroupColums->createWidget<Drag<float>>("W", ImGuiDataType_Float, width, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto size = rectTransform->getSize();
            size.X(val[0]);
            rectTransform->setSize(size);
            rectTransform->onUpdate(0.f);
            });

        std::array height = { rectTransform->getSize().Y() };
        anchorGroupColums->createWidget<Drag<float>>("H", ImGuiDataType_Float, height, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
            auto rectTransform = dynamic_cast<RectTransform*>(m_component);
            auto size = rectTransform->getSize();
            size.Y(val[0]);
            rectTransform->setSize(size);
            rectTransform->onUpdate(0.f);
            });
    }
}

void RectTransformEditorComponent::drawTransform() {
    if (m_rectTransformGroup == nullptr) return;
    m_rectTransformGroup->removeAllPlugins();
    m_rectTransformGroup->removeAllWidgets();

    auto rectTransform = dynamic_cast<RectTransform*>(m_component);
    if (rectTransform == nullptr)
        return;

    std::array pivot = { rectTransform->getPivot().X(), rectTransform->getPivot().Y() };
    m_rectTransformGroup->createWidget<Drag<float, 2>>("Pivot", ImGuiDataType_Float, pivot, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = dynamic_cast<RectTransform*>(m_component);
        rectTransform->setPivot({ val[0], val[1] });
        rectTransform->onUpdate(0.f);
        m_dirtyFlagSupport = 3;
        dirty();
        });

    Vec3 euler;
    vmath_quatToEuler(rectTransform->getRotation().P(), euler.P());
    std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
    m_rectTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = dynamic_cast<RectTransform*>(m_component);
        Quat quat;
        float rad[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
        vmath_eulerToQuat(rad, quat.P());
        rectTransform->setRotation(quat);
        rectTransform->onUpdate(0.f);
        m_dirtyFlagSupport = 3;
        dirty();
        });

    std::array scale = { rectTransform->getScale().X(), rectTransform->getScale().Y(), rectTransform->getScale().Z() };
    m_rectTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(RectTransformEditorComponent::onTransformChanged, this));
        auto rectTransform = dynamic_cast<RectTransform*>(m_component);
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
        if (m_targetObject != nullptr)
        {
            m_dirtyFlagSupport = 2;
            dirty();
        }
    });
}

NS_IGE_END