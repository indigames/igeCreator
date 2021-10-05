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

    m_targetAddedEventId = Editor::getTargetAddedEvent().addListener(std::bind(&TransformEditorComponent::onTargetAdded, this, std::placeholders::_1));
    m_targetRemovedEventId = Editor::getTargetRemovedEvent().addListener(std::bind(&TransformEditorComponent::onTargetRemoved, this, std::placeholders::_1));
    m_targetClearedEventId = Editor::getTargetClearedEvent().addListener(std::bind(&TransformEditorComponent::onTargetCleared, this));
}

TransformEditorComponent::~TransformEditorComponent() {
    m_localTransformGroup = nullptr;
    m_worldTransformGroup = nullptr;

    Editor::getTargetAddedEvent().removeListener(m_targetAddedEventId);
    Editor::getTargetRemovedEvent().removeListener(m_targetRemovedEventId);
    Editor::getTargetClearedEvent().removeListener(m_targetClearedEventId);

    if (!m_lastTarget.expired() && m_listenerId != (uint64_t)-1) {
        m_lastTarget.lock()->getTransformChangedEvent().removeListener(m_listenerId);
        m_listenerId = -1;
    }
    m_lastTarget.reset();
}

bool TransformEditorComponent::setComponent(std::shared_ptr<Component> component) {
    bool valid = EditorComponent::setComponent(component);
    if(valid) updateTarget();
    return valid;
}

//! Target events
void TransformEditorComponent::onTargetAdded(const std::shared_ptr<SceneObject>& object) {
    updateTarget();
}

void TransformEditorComponent::onTargetRemoved(const std::shared_ptr<SceneObject>& object) {
    updateTarget();
}

void TransformEditorComponent::updateTarget() {
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
                m_listenerId = m_lastTarget.lock()->getTransformChangedEvent().addListener(CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
            }
        }
    }
}

void TransformEditorComponent::onTargetCleared() {
    if (!m_lastTarget.expired() && m_listenerId != (uint64_t)-1) {
        m_lastTarget.lock()->getTransformChangedEvent().removeListener(m_listenerId);
        m_listenerId = -1;
    }
    m_lastTarget.reset();
}

void TransformEditorComponent::onInspectorUpdate() {
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

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
}

void TransformEditorComponent::drawLocalTransformComponent() {
    if (m_localTransformGroup == nullptr) {
        m_localTransformGroup = m_group->createWidget<Group>("LocalTransformGroup", false);
    }
    m_localTransformGroup->removeAllWidgets();
    m_localTransformGroup->createWidget<Label>("Local");

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto position = comp->getProperty<Vec3>("pos", Vec3(NAN, NAN, NAN));
    std::array pos = { position.X(), position.Y(), position.Z() };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
        m_dirtyFlag = 2;
        auto pos = getComponent<CompoundComponent>()->getProperty<Vec3>("pos", Vec3(NAN, NAN, NAN));
        if ((std::isnan(pos[0]) && !std::isnan(val[0]))
            || (std::isnan(pos[1]) && !std::isnan(val[1]))
            || (std::isnan(pos[2]) && !std::isnan(val[2]))) {
            m_dirtyFlag = 0;
        }
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        getComponent<CompoundComponent>()->setProperty("pos", Vec3(val[0], val[1], val[2]));
        getComponent<CompoundComponent>()->setDirty();
        Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
        setDirty();
    });

    auto euler = comp->getProperty<Vec3>("rot", Vec3(NAN, NAN, NAN));
    std::array rotArr = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rotArr)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        m_dirtyFlag = 2;
        auto euler = getComponent<CompoundComponent>()->getProperty<Vec3>("rot", Vec3(NAN, NAN, NAN));
        if ((std::isnan(euler[0]) && !std::isnan(val[0]))
            || (std::isnan(euler[1]) && !std::isnan(val[1]))
            || (std::isnan(euler[2]) && !std::isnan(val[2]))) {
            m_dirtyFlag = 0;
        }
        getComponent<CompoundComponent>()->setProperty("rot", Vec3(DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2])));
        getComponent<CompoundComponent>()->setDirty();
        Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
        setDirty();
    });

    auto scale = comp->getProperty<Vec3>("scale", Vec3(NAN, NAN, NAN));
    std::array scaleArr = { scale.X(), scale.Y(), scale.Z() };
    m_localTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scaleArr)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        m_dirtyFlag = 2;
        auto scale = getComponent<CompoundComponent>()->getProperty<Vec3>("scale", Vec3(NAN, NAN, NAN));
        if ((std::isnan(scale[0]) && !std::isnan(val[0]))
            || (std::isnan(scale[1]) && !std::isnan(val[1]))
            || (std::isnan(scale[2]) && !std::isnan(val[2]))) {
            m_dirtyFlag = 0;
        }
        getComponent<CompoundComponent>()->setProperty("scale", Vec3(val[0], val[1], val[2]));
        getComponent<CompoundComponent>()->setDirty();
        Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
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
        auto pos = getComponent<CompoundComponent>()->getProperty<Vec3>("wpos", Vec3(NAN, NAN, NAN));
        if ((std::isnan(pos[0]) && !std::isnan(val[0]))
            || (std::isnan(pos[1]) && !std::isnan(val[1]))
            || (std::isnan(pos[2]) && !std::isnan(val[2]))) {
            m_dirtyFlag = 0;
        }
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        getComponent<CompoundComponent>()->setProperty("wpos", Vec3(val[0], val[1], val[2]));
        getComponent<CompoundComponent>()->setDirty();
        Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
        setDirty();
    });

    auto euler = comp->getProperty<Vec3>("wrot", Vec3(NAN, NAN, NAN));
    std::array rotArr = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
    m_worldTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rotArr)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        m_dirtyFlag = 1;
        auto euler = getComponent<CompoundComponent>()->getProperty<Vec3>("wrot", Vec3(NAN, NAN, NAN));
        if ((std::isnan(euler[0]) && !std::isnan(val[0]))
            || (std::isnan(euler[1]) && !std::isnan(val[1]))
            || (std::isnan(euler[2]) && !std::isnan(val[2]))) {
            m_dirtyFlag = 0;
        }
        getComponent<CompoundComponent>()->setProperty("wrot", Vec3(DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2])));
        getComponent<CompoundComponent>()->setDirty();
        Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
        setDirty();
    });

    auto scale = comp->getProperty<Vec3>("wscale", Vec3(NAN, NAN, NAN));
    std::array scaleArr = { scale.X(), scale.Y(), scale.Z() };
    m_worldTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scaleArr)->getOnDataChangedEvent().addListener([this](auto val) {
        IgnoreTransformEventScope scope(m_lastTarget.lock().get(), m_listenerId, CALLBACK_1(TransformEditorComponent::onTransformChanged, this));
        m_dirtyFlag = 1;
        auto scale = getComponent<CompoundComponent>()->getProperty<Vec3>("wscale", Vec3(NAN, NAN, NAN));
        if ((std::isnan(scale[0]) && !std::isnan(val[0]))
            || (std::isnan(scale[1]) && !std::isnan(val[1]))
            || (std::isnan(scale[2]) && !std::isnan(val[2]))) {
            m_dirtyFlag = 0;
        }
        getComponent<CompoundComponent>()->setProperty("wscale", Vec3(val[0], val[1], val[2]));
        getComponent<CompoundComponent>()->setDirty();
        Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
        setDirty();
    });
}

void TransformEditorComponent::onTransformChanged(SceneObject& sceneObject)
{
    // Just redraw the transform in Inspector
    TaskManager::getInstance()->addTask([this]() {
        m_dirtyFlag = 0;
        auto comp = getComponent<CompoundComponent>();
        if (comp) {
            comp->setDirty();
            Editor::getCanvas()->getEditorScene()->getGizmo()->updateTargetNode();
        }
        setDirty();
    });
}

NS_IGE_END