#include "core/panels/components/TransformEditorComponent.h"

#include "components/TransformComponent.h"
#include <core/layout/Group.h>

#include "core/widgets/Widgets.h"
#include <core/panels/Inspector.h>

#include "core/task/TaskManager.h"

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/InspectorEditor.h"
NS_IGE_BEGIN

TransformEditorComponent::TransformEditorComponent() {
    m_localTransformGroup = nullptr;
    m_worldTransformGroup = nullptr;
    m_dirtyFlag = 0;
}

TransformEditorComponent::~TransformEditorComponent()
{
    if (m_localTransformGroup) {
        m_localTransformGroup->removeAllWidgets();
        m_localTransformGroup->removeAllPlugins();
        m_localTransformGroup = nullptr;
    }
    if (m_worldTransformGroup) {
        m_worldTransformGroup->removeAllWidgets();
        m_worldTransformGroup->removeAllPlugins();
        m_worldTransformGroup = nullptr;
    }
}

void TransformEditorComponent::setTargetObject(const std::shared_ptr<SceneObject>& obj )
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
            auto listenerId = m_targetObject->getTransformChangedEvent().addListener(CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
            Editor::getInstance()->getCanvas()->getInspector()->setTransformListenerId(listenerId);
        }
    }
}

bool TransformEditorComponent::isSafe(Component* comp)
{
	return dynamic_cast<TransformComponent*>(comp);
}

void TransformEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;
    
    if(m_localTransformGroup == nullptr)
        m_localTransformGroup = m_group->createWidget<Group>("LocalTransformGroup", false);
    if(m_worldTransformGroup == nullptr)
        m_worldTransformGroup = m_group->createWidget<Group>("WorldTransformGroup", false);
    
    switch (m_dirtyFlag) {
    case 0:
        drawLocalTransformComponent();
        drawWorldTransformComponent();
        break;
    case 1:
        drawLocalTransformComponent();
        break;
    case 2:
        drawWorldTransformComponent();
        break;
    default:
        break;
    }
    EditorComponent::redraw();
}

void TransformEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    auto transform = dynamic_cast<TransformComponent*>(m_component);
    if (transform == nullptr)
        return;

    m_localTransformGroup = m_group->createWidget<Group>("LocalTransformGroup", false);
    drawLocalTransformComponent();

    m_worldTransformGroup = m_group->createWidget<Group>("WorldTransformGroup", false);
    drawWorldTransformComponent();

    
}

void TransformEditorComponent::drawLocalTransformComponent() {

    if (m_localTransformGroup == nullptr)
        return;

    m_localTransformGroup->removeAllWidgets();
    m_localTransformGroup->removeAllPlugins();

    auto transform = dynamic_cast<TransformComponent*>(m_component);
    if (transform == nullptr)
        return;

    m_localTransformGroup->createWidget<Label>("Local");

    std::array pos = { transform->getPosition().X(), transform->getPosition().Y(), transform->getPosition().Z() };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        auto transform = dynamic_cast<TransformComponent*>(m_component);
        transform->setPosition({ val[0], val[1], val[2] });
        transform->onUpdate(0.f);
        m_dirtyFlag = 2;
        dirty();
        });

    Vec3 euler;
    vmath_quatToEuler(transform->getRotation().P(), euler.P());
    std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        auto transform = dynamic_cast<TransformComponent*>(m_component);
        Quat quat;
        float rad[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
        vmath_eulerToQuat(rad, quat.P());
        transform->setRotation(quat);
        transform->onUpdate(0.f);
        m_dirtyFlag = 2;
        dirty();
        });

    std::array scale = { transform->getScale().X(), transform->getScale().Y(), transform->getScale().Z() };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        auto transform = dynamic_cast<TransformComponent*>(m_component);
        transform->setScale({ val[0], val[1], val[2] });
        transform->onUpdate(0.f);
        m_dirtyFlag = 2;
        dirty();
        });
}

void TransformEditorComponent::drawWorldTransformComponent() {
    if (m_worldTransformGroup == nullptr)
        return;

    m_worldTransformGroup->removeAllWidgets();
    m_worldTransformGroup->removeAllPlugins();
    
    auto transform = dynamic_cast<TransformComponent*>(m_component);
    if (transform == nullptr)
        return;

    m_worldTransformGroup->createWidget<Label>("World");
    std::array pos = { transform->getWorldPosition().X(), transform->getWorldPosition().Y(), transform->getWorldPosition().Z() };
    m_worldTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        auto transform = dynamic_cast<TransformComponent*>(m_component);
        transform->setWorldPosition({ val[0], val[1], val[2] });
        transform->onUpdate(0.f);
        m_dirtyFlag = 1;
        dirty();
        });
    
    Vec3 euler;
    vmath_quatToEuler(transform->getWorldRotation().P(), euler.P());
    std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
    m_worldTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        auto transform = dynamic_cast<TransformComponent*>(m_component);
        Quat quat;
        float rad[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
        vmath_eulerToQuat(rad, quat.P());
        transform->setWorldRotation(quat);
        transform->onUpdate(0.f);
        m_dirtyFlag = 1;
        dirty();
        });

    std::array scale = { transform->getWorldScale().X(), transform->getWorldScale().Y(), transform->getWorldScale().Z() };
    m_worldTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_targetObject, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        auto transform = dynamic_cast<TransformComponent*>(m_component);
        transform->setWorldScale({ val[0], val[1], val[2] });
        transform->onUpdate(0.f);
        m_dirtyFlag = 1;
        dirty();
        });
}

void TransformEditorComponent::onTransformChanged(SceneObject& sceneObject)
{
    // Just redraw the transform in Inspector
    TaskManager::getInstance()->addTask([this]() {
        if (m_targetObject != nullptr)
        {
            m_dirtyFlag = 0;
            dirty();
        }
        });
}

NS_IGE_END