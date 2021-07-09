#include "core/scene/components/TransformEditorComponent.h"
#include "core/scene/CompoundComponent.h"

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
    m_localTransformGroup = nullptr;
    m_worldTransformGroup = nullptr;
}

bool TransformEditorComponent::setComponent(std::shared_ptr<Component> component)
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
                m_listenerId = newTarget->getTransformChangedEvent().addListener(CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
            }
        }
    }
    return EditorComponent::setComponent(component);
}

void TransformEditorComponent::onInspectorUpdate()
{
    switch (m_dirtyFlag)
    {
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
}

void TransformEditorComponent::drawLocalTransformComponent()
{
    if (m_localTransformGroup == nullptr) {
        m_localTransformGroup = m_group->createWidget<Group>("LocalTransformGroup", false);
    }
    m_localTransformGroup->removeAllWidgets();
    m_localTransformGroup->createWidget<Label>("Local");

    auto comp = getComponent<CompoundComponent>();
    auto position = comp->getProperty<Vec3>("pos", Vec3(NAN, NAN, NAN));
    std::array pos = { position.X(), position.Y(), position.Z() };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
        m_dirtyFlag = 2;
        if (std::isnan(val[0]) || std::isnan(val[1]) || std::isnan(val[2])) {
            m_dirtyFlag = 0;
            val[0] = val[1] = val[2] = 0.f;
        }
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        getComponent<CompoundComponent>()->setProperty("pos", Vec3(val[0], val[1], val[2]));
        setDirty();
    });

    auto rot = comp->getProperty<Quat>("rot", Quat(NAN, NAN, NAN, NAN));
    Vec3 euler;
    vmath_quatToEuler(rot.P(), euler.P());
    std::array rotArr = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rotArr)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        m_dirtyFlag = 2;
        if (std::isnan(val[0]) || std::isnan(val[1]) || std::isnan(val[2])) {
            m_dirtyFlag = 0;
            val[0] = val[1] = val[2] = 0.f;
        }        
        float rot[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
        Quat rotQuat;
        vmath_eulerToQuat(rot, rotQuat.P());
        getComponent<CompoundComponent>()->setProperty("rot", rotQuat);
        setDirty();
    });

    auto scale = comp->getProperty<Vec3>("scale", Vec3(NAN, NAN, NAN));
    std::array scaleArr = { scale.X(), scale.Y(), scale.Z() };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scaleArr)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        m_dirtyFlag = 2;
        if (std::isnan(val[0]) || std::isnan(val[1]) || std::isnan(val[2])) {
            m_dirtyFlag = 0;
            val[0] = val[1] = val[2] = 0.f;
        }
        getComponent<CompoundComponent>()->setProperty("scale", Vec3(val[0], val[1], val[2]));
        setDirty();
    });
}

void TransformEditorComponent::drawWorldTransformComponent() {
    if (m_worldTransformGroup == nullptr) {
        m_worldTransformGroup = m_group->createWidget<Group>("WorldTransformGroup", false);
    }
    m_worldTransformGroup->removeAllWidgets();
    m_worldTransformGroup->createWidget<Label>("World");

    auto comp = getComponent<CompoundComponent>();
    auto position = comp->getProperty<Vec3>("wpos", Vec3(NAN, NAN, NAN));
    std::array pos = { position.X(), position.Y(), position.Z() };
    m_worldTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
        m_dirtyFlag = 1;
        if (std::isnan(val[0]) || std::isnan(val[1]) || std::isnan(val[2])) {
            m_dirtyFlag = 0;
            val[0] = val[1] = val[2] = 0.f;
        }
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        getComponent<CompoundComponent>()->setProperty("wpos", Vec3(val[0], val[1], val[2]));
        setDirty();
    });

    auto rot = comp->getProperty<Quat>("wrot", Quat(NAN, NAN, NAN, NAN));
    Vec3 euler;
    vmath_quatToEuler(rot.P(), euler.P());
    std::array rotArr = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
    m_worldTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rotArr)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        m_dirtyFlag = 1;
        if (std::isnan(val[0]) || std::isnan(val[1]) || std::isnan(val[2])) {
            m_dirtyFlag = 0;
            val[0] = val[1] = val[2] = 0.f;
        }
        float rot[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
        Quat rotQuat;
        vmath_eulerToQuat(rot, rotQuat.P());
        getComponent<CompoundComponent>()->setProperty("wrot", rotQuat);
        setDirty();
    });

    auto scale = comp->getProperty<Vec3>("wscale", Vec3(NAN, NAN, NAN));
    std::array scaleArr = { scale.X(), scale.Y(), scale.Z() };
    m_worldTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scaleArr)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(getComponent(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        m_dirtyFlag = 1;
        if (std::isnan(val[0]) || std::isnan(val[1]) || std::isnan(val[2])) {
            m_dirtyFlag = 0;
            val[0] = val[1] = val[2] = 0.f;
        }
        getComponent<CompoundComponent>()->setProperty("wscale", Vec3(val[0], val[1], val[2]));
        setDirty();
    });
}

void TransformEditorComponent::onTransformChanged(SceneObject& sceneObject)
{
    // Just redraw the transform in Inspector
    TaskManager::getInstance()->addTask([this]() {
        m_dirtyFlag = 0;
    });
}

NS_IGE_END