#include <imgui.h>

#include "core/layout/Columns.h"
#include "core/widgets/Label.h"
#include "core/widgets/TextField.h"
#include "core/widgets/CheckBox.h"
#include "core/widgets/ComboBox.h"
#include "core/widgets/Button.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Drag.h"
#include "core/widgets/Slider.h"
#include "core/widgets/Color.h"
#include "core/widgets/AnchorWidget.h"
#include "core/panels/Inspector.h"
#include "core/Editor.h"
#include "core/FileHandle.h"
#include "core/task/TaskManager.h"

#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"

#include <components/CameraComponent.h>
#include <components/TransformComponent.h>
#include <components/EnvironmentComponent.h>
#include <components/FigureComponent.h>
#include <components/SpriteComponent.h>
#include <components/ScriptComponent.h>
#include <components/light/AmbientLight.h>
#include <components/light/DirectionalLight.h>
#include <components/light/PointLight.h>
#include <components/light/SpotLight.h>
#include <components/gui/RectTransform.h>
#include <components/gui/Canvas.h>
#include <components/gui/UIImage.h>
#include <components/gui/UIText.h>
#include <components/gui/UITextField.h>
#include <components/physic/PhysicBox.h>
#include <components/physic/PhysicSphere.h>
#include <components/physic/PhysicCapsule.h>
#include <components/physic/PhysicMesh.h>
#include <components/physic/PhysicSoftBody.h>
#include <components/physic/FixedConstraint.h>
#include <components/physic/HingeConstraint.h>
#include <components/physic/SliderConstraint.h>
#include <components/physic/SpringConstraint.h>
#include <components/physic/Dof6SpringConstraint.h>
#include <components/audio/AudioSource.h>
#include <components/audio/AudioListener.h>
#include <components/particle/Particle.h>
#include <components/navigation/NavMesh.h>
#include <components/navigation/NavAgent.h>
#include <components/navigation/NavAgentManager.h>
#include <components/navigation/Navigable.h>
#include <components/navigation/DynamicNavMesh.h>
#include <components/navigation/NavObstacle.h>
#include <components/navigation/OffMeshLink.h>
#include <scene/Scene.h>
using namespace ige::scene;

#include <pyxieUtilities.h>
using namespace pyxie;

namespace ige::creator
{
    enum class ComponentType
    {
        Camera = 0,
        Environment,
        Figure,
        Sprite,
        Script,
        AmbientLight,
        DirectionalLight,
        PointLight,
        SpotLight,
        UIImage,
        UIText,
        UITextField,
        PhysicBox,
        PhysicSphere,
        PhysicCapsule,
        PhysicMesh,
        PhysicSoftBody,
        AudioSource,
        AudioListener,
        Particle,
        Navigable,
        NavMesh,
        NavAgent,
        DynamicNavMesh,
        NavObstacle,
        OffMeshLink,
    };

    Inspector::Inspector(const std::string &name, const Panel::Settings &settings)
        : Panel(name, settings)
    {
    }

    Inspector::~Inspector()
    {
        m_targetObject = nullptr;

        clear();
    }

    void Inspector::initialize()
    {
        clear();

        if (m_targetObject == nullptr)
            return;

        m_headerGroup = createWidget<Group>("Inspector_Header", false);

        // Object info
        auto columns = m_headerGroup->createWidget<Columns<2>>();
        columns->createWidget<TextField>("ID", std::to_string(m_targetObject->getId()).c_str(), true);
        columns->createWidget<CheckBox>("Active", m_targetObject->isActive())->getOnDataChangedEvent().addListener([this](bool active) {
            if (m_targetObject)
                m_targetObject->setActive(active);
        });
        m_headerGroup->createWidget<TextField>("Name", m_targetObject->getName().c_str())->getOnDataChangedEvent().addListener([this](auto name) {
            if (m_targetObject)
                m_targetObject->setName(name);
        });

        // Create component selection
        m_createCompCombo = m_headerGroup->createWidget<ComboBox>();
        m_createCompCombo->setEndOfLine(false);
        m_createCompCombo->addChoice((int)ComponentType::Camera, "Camera");

        if(m_targetObject->getScene()->isDirectionalLightAvailable())
            m_createCompCombo->addChoice((int)ComponentType::DirectionalLight, "Directional Light");

        if (m_targetObject->getScene()->isPointLightAvailable())
            m_createCompCombo->addChoice((int)ComponentType::PointLight, "Point Light");

        if (m_targetObject->getScene()->isSpotLightAvailable())
            m_createCompCombo->addChoice((int)ComponentType::SpotLight, "Spot Light");

        // Scene Object
        if (!m_targetObject->isGUIObject())
        {
            m_createCompCombo->addChoice((int)ComponentType::Figure, "Figure");
            m_createCompCombo->addChoice((int)ComponentType::Sprite, "Sprite");

            if (m_targetObject->getComponent<PhysicObject>() == nullptr && m_targetObject->getComponent<PhysicSoftBody>() == nullptr)
            {
                m_createCompCombo->addChoice((int)ComponentType::PhysicBox, "PhysicBox");
                m_createCompCombo->addChoice((int)ComponentType::PhysicSphere, "PhysicSphere");
                m_createCompCombo->addChoice((int)ComponentType::PhysicCapsule, "PhysicCapsule");
                m_createCompCombo->addChoice((int)ComponentType::PhysicMesh, "PhysicMesh");
                if (m_targetObject->getComponent<FigureComponent>())
                    m_createCompCombo->addChoice((int)ComponentType::PhysicSoftBody, "PhysicSoftBody");
            }
        }
        else // GUI Object
        {
            m_createCompCombo->addChoice((int)ComponentType::UIImage, "UIImage");
            m_createCompCombo->addChoice((int)ComponentType::UIText, "UIText");
            m_createCompCombo->addChoice((int)ComponentType::UITextField, "UITextField");
        }

        // Script component
        m_createCompCombo->addChoice((int)ComponentType::Script, "Script Component");

        // Audio source
        m_createCompCombo->addChoice((int)ComponentType::AudioSource, "Audio Source");

        // Audio listener
        m_createCompCombo->addChoice((int)ComponentType::AudioListener, "Audio Listener");

        // Particle
        m_createCompCombo->addChoice((int)ComponentType::Particle, "Particle");

        // Navigation
        if (!m_targetObject->getComponent<NavMesh>() && !m_targetObject->getComponent<DynamicNavMesh>())
        {
            m_createCompCombo->addChoice((int)ComponentType::NavMesh, "NavMesh");
            m_createCompCombo->addChoice((int)ComponentType::DynamicNavMesh, "DynamicNavMesh");
        }
        m_createCompCombo->addChoice((int)ComponentType::Navigable, "Navigable");
        m_createCompCombo->addChoice((int)ComponentType::NavAgent, "NavAgent");
        m_createCompCombo->addChoice((int)ComponentType::NavObstacle, "NavObstacle");
        m_createCompCombo->addChoice((int)ComponentType::OffMeshLink, "OffMeshLink");

        auto createCompButton = m_headerGroup->createWidget<Button>("Add", ImVec2(64.f, 0.f));
        createCompButton->getOnClickEvent().addListener([this](auto widget) {
            switch (m_createCompCombo->getSelectedIndex())
            {
            case (int)ComponentType::Camera:
                m_targetObject->addComponent<CameraComponent>("camera");
                if (!m_targetObject->getComponent<FigureComponent>())
                    m_targetObject->addComponent<FigureComponent>("figure/camera.pyxf")->setSkipSerialize(true);
                break;
            case (int)ComponentType::Environment:
                m_targetObject->addComponent<EnvironmentComponent>();
                break;
            case (int)ComponentType::Script:
                m_targetObject->addComponent<ScriptComponent>();
                break;
            case (int)ComponentType::AmbientLight:
                m_targetObject->addComponent<AmbientLight>();
                break;
            case (int)ComponentType::DirectionalLight:
                m_targetObject->addComponent<DirectionalLight>();
                if (!m_targetObject->getComponent<FigureComponent>() && !m_targetObject->getComponent<SpriteComponent>())
                    m_targetObject->addComponent<SpriteComponent>("sprite/sun", Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                break;
            case (int)ComponentType::PointLight:
                m_targetObject->addComponent<PointLight>();
                if (!m_targetObject->getComponent<FigureComponent>() && !m_targetObject->getComponent<SpriteComponent>())
                    m_targetObject->addComponent<SpriteComponent>("sprite/light", Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                break;
            case (int)ComponentType::SpotLight:
                m_targetObject->addComponent<SpotLight>();
                if (!m_targetObject->getComponent<FigureComponent>() && !m_targetObject->getComponent<SpriteComponent>())
                    m_targetObject->addComponent<SpriteComponent>("sprite/spot-light", Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                break;
            case (int)ComponentType::Figure:
                m_targetObject->addComponent<FigureComponent>();
                break;
            case (int)ComponentType::Sprite:
                m_targetObject->addComponent<SpriteComponent>();
                break;
            case (int)ComponentType::UIImage:
                m_targetObject->addComponent<UIImage>();
                break;
            case (int)ComponentType::UIText:
                m_targetObject->addComponent<UIText>("Text", "fonts/Manjari-Regular.ttf");
                break;
            case (int)ComponentType::UITextField:
                m_targetObject->addComponent<UITextField>("TextField");
                break;
            case (int)ComponentType::PhysicBox:
                m_targetObject->addComponent<PhysicBox>();
                break;
            case (int)ComponentType::PhysicSphere:
                m_targetObject->addComponent<PhysicSphere>();
                break;
            case (int)ComponentType::PhysicCapsule:
                m_targetObject->addComponent<PhysicCapsule>();
                break;
            case (int)ComponentType::PhysicMesh:
                m_targetObject->addComponent<PhysicMesh>();
                break;
            case (int)ComponentType::PhysicSoftBody:
                m_targetObject->addComponent<PhysicSoftBody>();
                break;
            case (int)ComponentType::AudioSource:
                m_targetObject->addComponent<AudioSource>();
                break;
            case (int)ComponentType::AudioListener:
                m_targetObject->addComponent<AudioListener>();
                break;
            case (int)ComponentType::Particle:
                m_targetObject->addComponent<Particle>();
                break;
            case (int)ComponentType::Navigable:
                m_targetObject->addComponent<Navigable>();
                break;
            case (int)ComponentType::NavMesh:
                m_targetObject->addComponent<NavMesh>();
                break;
            case (int)ComponentType::NavAgent:
                m_targetObject->addComponent<NavAgent>();
                break;
            
            case (int)ComponentType::DynamicNavMesh:
                m_targetObject->addComponent<DynamicNavMesh>();
                break;
            
            case (int)ComponentType::NavObstacle:
                m_targetObject->addComponent<NavObstacle>();
                break;
            
            case (int)ComponentType::OffMeshLink:
                m_targetObject->addComponent<OffMeshLink>();
                break;
            }
            redraw();
        });

        // Component
        m_headerGroup->createWidget<Separator>();
        m_componentGroup = createWidget<Group>("Inspector_Components", false);
        std::for_each(m_targetObject->getComponents().begin(), m_targetObject->getComponents().end(), [this](auto &component) {
            auto closable = (component->getName() != "TransformComponent");
            auto header = m_componentGroup->createWidget<Group>(component->getName(), true, closable);
            header->getOnClosedEvent().addListener([this, &component]() {
                m_targetObject->removeComponent(component);
                redraw();
            });

            if (component->getName() == "TransformComponent")
            {
                m_localTransformGroup = header->createWidget<Group>("LocalTransformGroup", false);
                drawLocalTransformComponent();

                m_worldTransformGroup = header->createWidget<Group>("WorldTransformGroup", false);
                drawWorldTransformComponent();
            }
            else if (component->getName() == "CameraComponent")
            {
                m_cameraCompGroup = header->createWidget<Group>("CameraGroup", false);
                drawCameraComponent();
            }
            else if (component->getName() == "EnvironmentComponent")
            {
                m_environmentCompGroup = header->createWidget<Group>("EnvironmentGroup", false);
                drawEnvironmentComponent();
            }
            else if (component->getName() == "FigureComponent")
            {
                m_figureCompGroup = header->createWidget<Group>("FigureGroup", false);
                drawFigureComponent();
            }
            else if (component->getName() == "SpriteComponent")
            {
                m_spriteCompGroup = header->createWidget<Group>("SpriteGroup", false);
                drawSpriteComponent();
            }
            else if (component->getName() == "ScriptComponent")
            {
                m_scriptCompGroup = header->createWidget<Group>("ScriptGroup", false);
                drawScriptComponent();
            }
            else if (component->getName() == "RectTransform")
            {
                m_rectTransformGroup = header->createWidget<Group>("RectTransformGroup", false);
                drawRectTransform();
            }
            else if (component->getName() == "Canvas")
            {
                m_canvasGroup = header->createWidget<Group>("CanvasGroup", false);
                drawCanvas();
            }
            else if (component->getName() == "UIImage")
            {
                m_uiImageGroup = header->createWidget<Group>("UIImageGroup", false);
                drawUIImage();
            }
            else if (component->getName() == "UIText" || component->getName() == "UITextField")
            {
                m_uiTextGroup = header->createWidget<Group>("UITextGroup", false);
                drawUIText();
            }
            else if (component->getName() == "PhysicBox")
            {
                m_physicGroup = header->createWidget<Group>("PhysicGroup", false);
                drawPhysicBox();
            }
            else if (component->getName() == "PhysicSphere")
            {
                m_physicGroup = header->createWidget<Group>("PhysicGroup", false);
                drawPhysicSphere();
            }
            else if (component->getName() == "PhysicCapsule")
            {
                m_physicGroup = header->createWidget<Group>("PhysicGroup", false);
                drawPhysicCapsule();
            }
            else if (component->getName() == "PhysicMesh")
            {
                m_physicGroup = header->createWidget<Group>("PhysicGroup", false);
                drawPhysicMesh();
            }
            else if (component->getName() == "PhysicSoftBody")
            {
                m_physicGroup = header->createWidget<Group>("PhysicGroup", false);
                drawPhysicSoftBody();
            }
            else if (component->getName() == "AudioSource")
            {
                m_audioSourceGroup = header->createWidget<Group>("AudioSource", false);
                drawAudioSource();
            }
            else if (component->getName() == "AudioListener")
            {
                m_audioListenerGroup = header->createWidget<Group>("AudioListener", false);
                drawAudioListener();
            }
            else if (component->getName() == "AmbientLight")
            {
                m_ambientLightGroup = header->createWidget<Group>("AmbientLight", false);
                drawAmbientLight();
            }
            else if (component->getName() == "DirectionalLight")
            {
                m_directionalLightGroup = header->createWidget<Group>("DirectionalLight", false);
                drawDirectionalLight();
            }
            else if (component->getName() == "PointLight")
            {
                m_pointLightGroup = header->createWidget<Group>("PointLight", false);
                drawPointLight();
            }
            else if (component->getName() == "SpotLight")
            {
                m_spotLightGroup = header->createWidget<Group>("SpotLight", false);
                drawSpotLight();
            }
            else if (component->getName() == "Particle")
            {
                m_particleGroup = header->createWidget<Group>("Particle", false);
                drawParticle();
            }
            else if (component->getName() == "Navigable")
            {
                m_navigableGroup = header->createWidget<Group>("Navigable", false);
                drawNavigable();
            }
            else if (component->getName() == "NavMesh")
            {
                m_navMeshGroup = header->createWidget<Group>("NavMesh", false);
                drawNavMesh();
            }
            else if (component->getName() == "NavAgent")
            {
                m_navAgentGroup = header->createWidget<Group>("NavAgent", false);
                drawNavAgent();
            }
            else if (component->getName() == "NavAgentManager")
            {
                m_navAgentManagerGroup = header->createWidget<Group>("NavAgentManager", false);
                drawNavAgentManager();
            }        
            else if (component->getName() == "DynamicNavMesh")
            {
                m_navMeshGroup = header->createWidget<Group>("DynamicNavMesh", false);
                drawDynamicNavMesh();
            }        
            else if (component->getName() == "NavObstacle")
            {
                m_navObstacleGroup = header->createWidget<Group>("NavObstacle", false);
                drawNavObstacle();
            }        
            else if (component->getName() == "OffMeshLink")
            {
                m_offMeshLinkGroup = header->createWidget<Group>("OffMeshLink", false);
                drawOffMeshLink();
            }
        });
    }

    void Inspector::drawLocalTransformComponent()
    {
        if (m_localTransformGroup == nullptr)
            return;

        m_localTransformGroup->removeAllWidgets();
        auto transform = m_targetObject->getTransform();
        if (transform == nullptr)
            return;

        m_localTransformGroup->createWidget<Label>("Local");

        std::array pos = {transform->getPosition().X(), transform->getPosition().Y(), transform->getPosition().Z()};
        m_localTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto transform = m_targetObject->getTransform();
            transform->setPosition({val[0], val[1], val[2]});
            transform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

        Vec3 euler;
        vmath_quatToEuler(transform->getRotation().P(), euler.P());
        std::array rot = {RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z())};
        m_localTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto transform = m_targetObject->getTransform();
            Quat quat;
            float rad[3] = {DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2])};
            vmath_eulerToQuat(rad, quat.P());
            transform->setRotation(quat);
            transform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

        std::array scale = {transform->getScale().X(), transform->getScale().Y(), transform->getScale().Z()};
        m_localTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto transform = m_targetObject->getTransform();
            transform->setScale({val[0], val[1], val[2]});
            transform->onUpdate(0.f);
            drawWorldTransformComponent();
        });
    }

    void Inspector::drawWorldTransformComponent()
    {
        if (m_worldTransformGroup == nullptr)
            return;

        m_worldTransformGroup->removeAllWidgets();
        auto transform = m_targetObject->getTransform();
        if (transform == nullptr)
            return;

        m_worldTransformGroup->createWidget<Label>("World");
        std::array pos = {transform->getWorldPosition().X(), transform->getWorldPosition().Y(), transform->getWorldPosition().Z()};
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto transform = m_targetObject->getTransform();
            transform->setWorldPosition({val[0], val[1], val[2]});
            transform->onUpdate(0.f);
            drawLocalTransformComponent();
        });

        Vec3 euler;
        vmath_quatToEuler(transform->getWorldRotation().P(), euler.P());
        std::array rot = {RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z())};
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto transform = m_targetObject->getTransform();
            Quat quat;
            float rad[3] = {DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2])};
            vmath_eulerToQuat(rad, quat.P());
            transform->setWorldRotation(quat);
            transform->onUpdate(0.f);
            drawLocalTransformComponent();
        });

        std::array scale = {transform->getWorldScale().X(), transform->getWorldScale().Y(), transform->getWorldScale().Z()};
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto transform = m_targetObject->getTransform();
            transform->setWorldScale({val[0], val[1], val[2]});
            transform->onUpdate(0.f);
            drawLocalTransformComponent();
        });
    }

    void Inspector::drawCameraComponent()
    {
        if (m_cameraCompGroup == nullptr)
            return;
        auto camera = m_targetObject->getComponent<CameraComponent>();
        if (camera == nullptr)
            return;
        m_cameraCompGroup->removeAllWidgets();

        auto columns = m_cameraCompGroup->createWidget<Columns<3>>(120.f);
        // Orthographic
        std::array orthorW = {camera->getOrthoWidth()};
        columns->createWidget<Drag<float>>("OrtW", ImGuiDataType_Float, orthorW)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setOrthoWidth(val[0]);
        });
        std::array orthorH = {camera->getOrthoHeight()};
        columns->createWidget<Drag<float>>("OrtH", ImGuiDataType_Float, orthorH)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setOrthoHeight(val[0]);
        });
        columns->createWidget<CheckBox>("IsOrtho", camera->isOrthoProjection())->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setOrthoProjection(val);
        });

        // FOV - Near - Far
        std::array fov = {camera->getFieldOfView()};
        columns->createWidget<Drag<float>>("FOV", ImGuiDataType_Float, fov)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setFieldOfView(val[0]);
        });
        std::array camNear = {camera->getNearPlane()};
        columns->createWidget<Drag<float>>("Near", ImGuiDataType_Float, camNear)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setNearPlane(val[0]);
        });
        std::array camFar = {camera->getFarPlane()};
        columns->createWidget<Drag<float>>("Far", ImGuiDataType_Float, camFar)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setFarPlane(val[0]);
        });

        // Target
        auto drawCameraLockTarget = [this]() {
            m_cameraLockTargetGroup->removeAllWidgets();

            auto camera = m_targetObject->getComponent<CameraComponent>();
            if (camera->getLockOn())
            {
                std::array target = {camera->getTarget().X(), camera->getTarget().Y(), camera->getTarget().Z()};
                auto targetGroup = m_cameraLockTargetGroup->createWidget<Drag<float, 3>>("Target", ImGuiDataType_Float, target);
                targetGroup->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = m_targetObject->getComponent<CameraComponent>();
                    camera->setTarget({val[0], val[1], val[2]});
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                });
            }
            else
            {
                // Pan - Tilt - Roll
                auto columns = m_cameraLockTargetGroup->createWidget<Columns<3>>(120.f);
                std::array pan = {RADIANS_TO_DEGREES(camera->getPan())};
                columns->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan)->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = m_targetObject->getComponent<CameraComponent>();
                    camera->setPan(DEGREES_TO_RADIANS(val[0]));
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                });
                std::array tilt = {RADIANS_TO_DEGREES(camera->getTilt())};
                columns->createWidget<Drag<float>>("Tilt", ImGuiDataType_Float, tilt)->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = m_targetObject->getComponent<CameraComponent>();
                    camera->setTilt(DEGREES_TO_RADIANS(val[0]));
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                });
                std::array roll = {RADIANS_TO_DEGREES(camera->getRoll())};
                columns->createWidget<Drag<float>>("Roll", ImGuiDataType_Float, roll)->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = m_targetObject->getComponent<CameraComponent>();
                    camera->setRoll(DEGREES_TO_RADIANS(val[0]));
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                });
            }
        };

        m_cameraCompGroup->createWidget<CheckBox>("LockTarget", camera->getLockOn())->getOnDataChangedEvent().addListener([drawCameraLockTarget, this](auto locked) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            if (!locked)
            {
                auto transform = m_targetObject->getTransform();
                camera->setTarget(transform->getPosition() + Vec3(0.f, 0.f, -1.f));
            }
            else
            {
                camera->setPan(0.f);
                camera->setTilt(0.f);
                camera->setRoll(0.f);
            }
            camera->lockOnTarget(locked);
            drawLocalTransformComponent();
            drawWorldTransformComponent();
            drawCameraLockTarget();
        });

        m_cameraLockTargetGroup = m_cameraCompGroup->createWidget<Group>("LockTargetGroup", false);
        drawCameraLockTarget();

        // Width Based
        auto widthBasedColumns = m_cameraCompGroup->createWidget<Columns<2>>(180.f);
        widthBasedColumns->createWidget<CheckBox>("WidthBased", camera->isWidthBase())->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setWidthBase(val);
        });

        // Aspect Ratio
        std::array ratio = {camera->getAspectRatio()};
        widthBasedColumns->createWidget<Drag<float>>("Ratio", ImGuiDataType_Float, ratio)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setAspectRatio(val[0]);
        });
    }

    void Inspector::drawEnvironmentComponent()
    {
        if (m_environmentCompGroup == nullptr)
            return;
        m_environmentCompGroup->removeAllWidgets();
        auto environment = m_targetObject->getComponent<EnvironmentComponent>();
        if (environment == nullptr)
            return;

        // Shadow
        auto shadowGroup = m_environmentCompGroup->createWidget<Group>("Shadow");
        auto shadowColor = Vec4(environment->getShadowColor(), 1.f);
        shadowGroup->createWidget<Color>("Color", shadowColor)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowColor({val[0], val[1], val[2]});
        });

        std::array size = { environment->getShadowTextureSize()[0], environment->getShadowTextureSize()[1] };
        shadowGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, size, 1, 512, 4096)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowTextureSize(Vec2(val[0], val[1]));
        });

        std::array density = {environment->getShadowDensity()};
        shadowGroup->createWidget<Drag<float>>("Density", ImGuiDataType_Float, density, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowDensity(val[0]);
        });
        std::array wideness = {environment->getShadowWideness()};
        shadowGroup->createWidget<Drag<float>>("Wideness", ImGuiDataType_Float, wideness, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowWideness(val[0]);
        });
        std::array bias = { environment->getShadowBias() };
        shadowGroup->createWidget<Drag<float>>("Bias", ImGuiDataType_Float, bias, 0.0001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowBias(val[0]);
        });

        // Fog
        auto fogGroup = m_environmentCompGroup->createWidget<Group>("Fog");
        auto fogColor = Vec4(environment->getDistanceFogColor(), environment->getDistanceFogAlpha());
        fogGroup->createWidget<Color>("Color", fogColor)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogColor({val[0], val[1], val[2]});
            environment->setDistanceFogAlpha(val[3]);
        });

        auto fogColumn = fogGroup->createWidget<Columns<2>>(120.f);
        std::array fogNear = {environment->getDistanceFogNear()};
        fogColumn->createWidget<Drag<float>>("Near", ImGuiDataType_Float, fogNear, 0.01f, 0.1f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogNear(val[0]);
        });

        std::array fogFar = {environment->getDistanceFogFar()};
        fogColumn->createWidget<Drag<float>>("Far", ImGuiDataType_Float, fogFar, 0.01f, 0.1f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogFar(val[0]);
        });
    }

    void Inspector::drawFigureComponent()
    {
        if (m_figureCompGroup == nullptr)
            return;
        m_figureCompGroup->removeAllWidgets();

        auto figureComp = m_targetObject->getComponent<FigureComponent>();
        if (figureComp == nullptr)
            return;

        auto txtPath = m_figureCompGroup->createWidget<TextField>("Path", figureComp->getPath());
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto figureComp = m_targetObject->getComponent<FigureComponent>();
            figureComp->setPath(txt);
        });

        for (const auto &type : GetFileExtensionSuported(E_FileExts::Figure))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto figureComp = m_targetObject->getComponent<FigureComponent>();
                figureComp->setPath(txt);
                redraw();
            });
        }

        m_figureCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", {"Figure (*.pyxf)", "*.pyxf"}).result();
            if (files.size() > 0)
            {
                auto figureComp = m_targetObject->getComponent<FigureComponent>();
                figureComp->setPath(files[0]);
                redraw();
            }
        });

        auto figColumn = m_figureCompGroup->createWidget<Columns<2>>();

        figColumn->createWidget<CheckBox>("Fog", figureComp->isFogEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto figureComp = m_targetObject->getComponent<FigureComponent>();
            figureComp->setFogEnabled(val);
        });

        figColumn->createWidget<CheckBox>("CullFace", figureComp->isCullFaceEnable())->getOnDataChangedEvent().addListener([this](bool val) {
            auto figureComp = m_targetObject->getComponent<FigureComponent>();
            figureComp->setCullFaceEnable(val);
        });

        figColumn->createWidget<CheckBox>("Z-Test", figureComp->isDepthTestEnable())->getOnDataChangedEvent().addListener([this](bool val) {
            auto figureComp = m_targetObject->getComponent<FigureComponent>();
            figureComp->setDepthTestEnable(val);
        });
    
        figColumn->createWidget<CheckBox>("Z-Write", figureComp->isDepthWriteEnable())->getOnDataChangedEvent().addListener([this](bool val) {
            auto figureComp = m_targetObject->getComponent<FigureComponent>();
            figureComp->setDepthWriteEnable(val);
        });

        figColumn->createWidget<CheckBox>("AlphaBlend", figureComp->isAlphaBlendingEnable())->getOnDataChangedEvent().addListener([this](bool val) {
            auto figureComp = m_targetObject->getComponent<FigureComponent>();
            figureComp->setAlphaBlendingEnable(val);
            redraw();
        });

        if (figureComp->isAlphaBlendingEnable())
        {
            std::array val = { figureComp->getAlphaBlendingOp() };
            m_figureCompGroup->createWidget<Drag<int>>("AlphaBlendOp", ImGuiDataType_S32, val, 1, 0, 3)->getOnDataChangedEvent().addListener([this](auto val) {
                auto figureComp = m_targetObject->getComponent<FigureComponent>();
                figureComp->setAlphaBlendingOp(val[0]);
            });
        }

        auto figure = figureComp->getFigure();
        if (figure)
        {
            if (figure->NumMeshes() > 0)
            {
                auto meshGroup = m_figureCompGroup->createWidget<Group>("Mesh");
                auto meshColumn = meshGroup->createWidget<Columns<3>>();
                for (int i = 0; i < figure->NumMeshes(); i++)
                {
                    auto mesh = figure->GetMesh(i);
                    auto txtName = meshColumn->createWidget<TextField>("", figure->GetMeshName(i), true);
                    txtName->addPlugin<DDSourcePlugin<int>>(EDragDropID::MESH, figure->GetMeshName(i), i);
                    meshColumn->createWidget<TextField>("V", std::to_string(mesh->numVerticies).c_str(), true);
                    meshColumn->createWidget<TextField>("I", std::to_string(mesh->numIndices).c_str(), true);
                }
            }

            if (figure->NumJoints() > 0)
            {
                auto joinGroup = m_figureCompGroup->createWidget<Group>("Joint");
                for (int i = 0; i < figure->NumJoints(); i++)
                {
                    joinGroup->createWidget<TextField>("", figure->GetJointName(i), true);
                }
            }

            if (figure->NumMaterials() > 0)
            {
                auto materialGroup = m_figureCompGroup->createWidget<Group>("Material");
                for (int i = 0; i < figure->NumMaterials(); i++)
                {
                    const char* matName = figure->GetMaterialName(i);
                    if (matName)
                    {
                        materialGroup->createWidget<Label>(matName);
                        int index = figure->GetMaterialIndex(GenerateNameHash(matName));
                        auto currMat = figure->GetMaterial(i);

                        for (auto j = 0; j < currMat->numParams; j++)
                        {
                            auto info = RenderContext::Instance().GetShaderParameterInfoByHash(currMat->params[j].hash);
                            if (!info)
                                continue;

                            auto infoName = info->name;
                            if ((currMat->params[j].type == ParamTypeFloat4))
                            {
                                auto color = Vec4(currMat->params[j].fValue[0], currMat->params[j].fValue[1], currMat->params[j].fValue[2], currMat->params[j].fValue[3]);
                                materialGroup->createWidget<Color>(info->name, color);
                            }
                            else if ((currMat->params[j].type == ParamTypeSampler))
                            {
                                auto textPath = currMat->params[j].sampler.tex->ResourceName();
                                materialGroup->createWidget<TextField>(info->name, textPath);
                            }
                            else if ((currMat->params[j].type == ParamTypeFloat))
                            {
                                std::array val = { currMat->params[j].fValue[0] };
                                materialGroup->createWidget<Drag<float>>(info->name, ImGuiDataType_Float, val);
                            }
                        }
                    }
                }
            }
        }
    }

    void Inspector::drawSpriteComponent()
    {
        if (m_spriteCompGroup == nullptr)
            return;
        m_spriteCompGroup->removeAllWidgets();

        auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
        if (spriteComp == nullptr)
            return;

        auto txtPath = m_spriteCompGroup->createWidget<TextField>("Path", spriteComp->getPath());
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setPath(txt);
        });

        for (const auto &type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
                spriteComp->setPath(txt);
                redraw();
            });
        }

        m_spriteCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", {"Texture (*.pyxi)", "*.pyxi"}).result();
            if (files.size() > 0)
            {
                auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
                spriteComp->setPath(files[0]);
                redraw();
            }
        });

        std::array size = {spriteComp->getSize().X(), spriteComp->getSize().Y()};
        m_spriteCompGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, size, 1.f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setSize({val[0], val[1]});
        });

        m_spriteCompGroup->createWidget<CheckBox>("Billboard", spriteComp->isBillboard())->getOnDataChangedEvent().addListener([this](bool val) {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setBillboard(val);
        });
    }

    void Inspector::drawScriptComponent()
    {
        if (m_scriptCompGroup == nullptr)
            return;
        m_scriptCompGroup->removeAllWidgets();

        auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
        if (scriptComp == nullptr)
            return;

        auto txtPath = m_scriptCompGroup->createWidget<TextField>("Path", scriptComp->getPath());
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
            scriptComp->setPath(txt);
        });

        for (const auto &type : GetFileExtensionSuported(E_FileExts::Script))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                scriptComp->setPath(txt);
                redraw();
            });
        }

        m_scriptCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", {"Script (*.py)", "*.py"}).result();
            if (files.size() > 0)
            {
                auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                scriptComp->setPath(files[0]);
                redraw();
            }
        });
    }

    void Inspector::drawRectTransformAnchor()
    {
        if (m_rectTransformAnchorGroup == nullptr)
            return;

        auto rectTransform = m_targetObject->getComponent<RectTransform>();
        if (rectTransform == nullptr)
            return;

        m_rectTransformAnchorGroup->removeAllWidgets();

        m_rectTransformAnchorGroup->createWidget<AnchorPresets>("AnchorPresets")->getOnClickEvent().addListener([this](const auto& widget) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            auto anchor = rectTransform->getAnchor();
            auto anchorWidget = (AnchorWidget*)widget;
            anchor[0] = anchorWidget->getAnchorMin().x;
            anchor[1] = anchorWidget->getAnchorMin().y;
            anchor[2] = anchorWidget->getAnchorMax().x;
            anchor[3] = anchorWidget->getAnchorMax().y;
            rectTransform->setAnchor(anchor);
            redraw();
        });

        auto anchorColumn = m_rectTransformAnchorGroup->createWidget<Columns<2>>();
        anchorColumn->setColumnWidth(0, 52.f);
        auto anchor = rectTransform->getAnchor();
        auto offset = rectTransform->getOffset();
        anchorColumn->createWidget<AnchorWidget>(ImVec2(anchor[0], anchor[1]), ImVec2(anchor[2], anchor[3]), false)->getOnClickEvent().addListener([](auto widget) {
            ImGui::OpenPopup("AnchorPresets");
        });

        auto anchorGroup = anchorColumn->createWidget<Group>("AnchorGroup", false, false);
        auto anchorGroupColums = anchorGroup->createWidget<Columns<3>>(-1.f, true, 52.f);

        if ((anchor[0] == 0.f && anchor[2] == 1.f) && (anchor[1] == 0.f && anchor[3] == 1.f))
        {
            std::array left = { offset[0] };
            anchorGroupColums->createWidget<Drag<float>>("L", ImGuiDataType_Float, left, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[0] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array top = { offset[1] };
            anchorGroupColums->createWidget<Drag<float>>("T", ImGuiDataType_Float, top, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[1] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = { rectTransform->getPosition().Z() };
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array right = { offset[2] };
            anchorGroupColums->createWidget<Drag<float>>("R", ImGuiDataType_Float, right, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[2] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array bottom = { offset[3] };
            anchorGroupColums->createWidget<Drag<float>>("B", ImGuiDataType_Float, bottom, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
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
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[0] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });
            std::array posY = { rectTransform->getPosition().Y() };
            anchorGroupColums->createWidget<Drag<float>>("Y", ImGuiDataType_Float, posY, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Y(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });
            std::array posZ = { rectTransform->getPosition().Z() };
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array right = { offset[2] };
            anchorGroupColums->createWidget<Drag<float>>("R", ImGuiDataType_Float, right, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[2] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array height = { rectTransform->getSize().Y() };
            anchorGroupColums->createWidget<Drag<float>>("H", ImGuiDataType_Float, height, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
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
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.X(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array top = { offset[1] };
            anchorGroupColums->createWidget<Drag<float>>("T", ImGuiDataType_Float, top, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[1] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = { rectTransform->getPosition().Z() };
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array width = { rectTransform->getSize().X() };
            anchorGroupColums->createWidget<Drag<float>>("W", ImGuiDataType_Float, width, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.X(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });

            std::array bottom = { offset[3] };
            anchorGroupColums->createWidget<Drag<float>>("B", ImGuiDataType_Float, bottom, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
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
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.X(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array posY = { rectTransform->getPosition().Y() };
            anchorGroupColums->createWidget<Drag<float>>("Y", ImGuiDataType_Float, posY, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Y(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = { rectTransform->getPosition().Z() };
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array width = { rectTransform->getSize().X() };
            anchorGroupColums->createWidget<Drag<float>>("W", ImGuiDataType_Float, width, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.X(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });

            std::array height = { rectTransform->getSize().Y() };
            anchorGroupColums->createWidget<Drag<float>>("H", ImGuiDataType_Float, height, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.Y(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });
        }
    }

    void Inspector::drawRectTransform()
    {
        if (m_rectTransformGroup == nullptr)
            return;
        m_rectTransformGroup->removeAllWidgets();

        auto rectTransform = m_targetObject->getComponent<RectTransform>();
        if (rectTransform == nullptr)
            return;

        if (m_rectTransformAnchorGroup)
        {
            m_rectTransformAnchorGroup->removeAllWidgets();
            m_rectTransformAnchorGroup->removeAllPlugins();
        }

        m_rectTransformAnchorGroup = m_rectTransformGroup->createWidget<Group>("RectTransformAnchorGroup", false);
        drawRectTransformAnchor();

        std::array pivot = {rectTransform->getPivot().X(), rectTransform->getPivot().Y()};
        m_rectTransformGroup->createWidget<Drag<float, 2>>("Pivot", ImGuiDataType_Float, pivot, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            rectTransform->setPivot({val[0], val[1]});
            rectTransform->onUpdate(0.f);
            drawRectTransformAnchor();
        });

        Vec3 euler;
        vmath_quatToEuler(rectTransform->getRotation().P(), euler.P());
        std::array rot = {RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z())};
        m_rectTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            Quat quat;
            float rad[3] = {DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2])};
            vmath_eulerToQuat(rad, quat.P());
            rectTransform->setRotation(quat);
            rectTransform->onUpdate(0.f);
            drawRectTransformAnchor();
        });

        std::array scale = {rectTransform->getScale().X(), rectTransform->getScale().Y(), rectTransform->getScale().Z()};
        m_rectTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            IgnoreTransformEventScope scope(m_targetObject, std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            rectTransform->setScale({val[0], val[1], val[2]});
            rectTransform->onUpdate(0.f);
            drawRectTransformAnchor();
        });
    }

    void Inspector::drawCanvas()
    {
        if (m_canvasGroup == nullptr)
            return;
        m_canvasGroup->removeAllWidgets();

        auto canvas = m_targetObject->getCanvas();
        if (canvas == nullptr)
            return;

        std::array size = {canvas->getDesignCanvasSize().X(), canvas->getDesignCanvasSize().Y()};
        m_canvasGroup->createWidget<Drag<float, 2>>("Design Size", ImGuiDataType_Float, size)->getOnDataChangedEvent().addListener([this](auto val) {
            auto canvas = m_targetObject->getCanvas();
            canvas->setDesignCanvasSize({val[0], val[1]});
        });

        std::array targetSize = {canvas->getTargetCanvasSize().X(), canvas->getTargetCanvasSize().Y()};
        m_canvasGroup->createWidget<Drag<float, 2>>("Target Size", ImGuiDataType_Float, targetSize)->getOnDataChangedEvent().addListener([this](auto val) {
            auto canvas = m_targetObject->getCanvas();
            canvas->setTargetCanvasSize({val[0], val[1]});
        });
    }

    void Inspector::drawUIImage()
    {
        if (m_uiImageGroup == nullptr)
            return;
        m_uiImageGroup->removeAllWidgets();

        auto uiImage = m_targetObject->getComponent<UIImage>();
        if (uiImage == nullptr)
            return;

        auto txtPath = m_uiImageGroup->createWidget<TextField>("Path", uiImage->getPath());
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiImage = m_targetObject->getComponent<UIImage>();
            uiImage->setPath(txt);
        });

        for (const auto &type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto uiImage = m_targetObject->getComponent<UIImage>();
                uiImage->setPath(txt);
                redraw();
            });
        }

        m_uiImageGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", {"Texture (*.pyxi)", "*.pyxi"}).result();
            if (files.size() > 0)
            {
                auto uiImage = m_targetObject->getComponent<UIImage>();
                uiImage->setPath(files[0]);
                redraw();
            }
        });
    }

    void Inspector::drawUIText()
    {
        if (m_uiTextGroup == nullptr)
            return;
        m_uiTextGroup->removeAllWidgets();

        auto uiText = m_targetObject->getComponent<UIText>();
        if (uiText == nullptr)
            return;

        auto txtText = m_uiTextGroup->createWidget<TextField>("Text", uiText->getText().c_str());
        txtText->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setText(txt);
        });

        auto txtFontPath = m_uiTextGroup->createWidget<TextField>("Font", uiText->getFontPath().c_str());
        txtFontPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setFontPath(txt);
        });
        txtFontPath->setEndOfLine(false);
        for (const auto &type : GetFileExtensionSuported(E_FileExts::Font))
        {
            txtFontPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto uiText = m_targetObject->getComponent<UIText>();
                uiText->setFontPath(txt);
            });
        }

        std::array size = {(float)uiText->getFontSize()};
        m_uiTextGroup->createWidget<Drag<float>>("Size", ImGuiDataType_Float, size, 1.f, 4.f, 128.f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setFontSize((int)val[0]);
        });

        auto color = uiText->getColor();
        m_uiTextGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto &color) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setColor({color[0], color[1], color[2], color[3]});
        });
    }

    //! Draw PhysicObject component
    void Inspector::drawPhysicObject()
    {
        if (m_physicGroup == nullptr)
            return;
        m_physicGroup->removeAllWidgets();

        auto physicComp = m_targetObject->getComponent<PhysicObject>();
        if (physicComp == nullptr)
            return;

        auto columns = m_physicGroup->createWidget<Columns<2>>();
         columns->createWidget<CheckBox>("Enable", physicComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setEnabled(val);
        });

        columns->createWidget<CheckBox>("Continous", physicComp->isCCD())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setCCD(val);
        });

        columns->createWidget<CheckBox>("Kinematic", physicComp->isKinematic())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setIsKinematic(val);
            redraw();
        });

        columns->createWidget<CheckBox>("Trigger", physicComp->isTrigger())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setIsTrigger(val);
        });

        std::array filterGroup = { physicComp->getCollisionFilterGroup() };
        m_physicGroup->createWidget<Drag<int>>("Collision Group", ImGuiDataType_S32, filterGroup, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setCollisionFilterGroup(val[0]);
        });

        std::array filterMask = { physicComp->getCollisionFilterMask() };
        m_physicGroup->createWidget<Drag<int>>("Collision Mask", ImGuiDataType_S32, filterMask, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setCollisionFilterMask(val[0]);
        });

        std::array mass = {physicComp->getMass()};
        m_physicGroup->createWidget<Drag<float>>("Mass", ImGuiDataType_Float, mass, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setMass(val[0]);
        });

        std::array friction = {physicComp->getFriction()};
        m_physicGroup->createWidget<Drag<float>>("Friction", ImGuiDataType_Float, friction, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setFriction(val[0]);
        });

        std::array restitution = {physicComp->getRestitution()};
        m_physicGroup->createWidget<Drag<float>>("Restitution", ImGuiDataType_Float, restitution, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setRestitution(val[0]);
        });

        std::array linearVelocity = {physicComp->getLinearVelocity().x(), physicComp->getLinearVelocity().y(), physicComp->getLinearVelocity().z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Linear Velocity", ImGuiDataType_Float, linearVelocity)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setLinearVelocity({val[0], val[1], val[2]});
        });

        std::array angularVelocity = {physicComp->getAngularVelocity().x(), physicComp->getAngularVelocity().y(), physicComp->getAngularVelocity().z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Angular Velocity", ImGuiDataType_Float, angularVelocity)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setAngularVelocity({val[0], val[1], val[2]});
        });

        std::array linearFactor = {physicComp->getLinearFactor().x(), physicComp->getLinearFactor().y(), physicComp->getLinearFactor().z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Linear Factor", ImGuiDataType_Float, linearFactor)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setLinearFactor({val[0], val[1], val[2]});
        });

        std::array angularFactor = {physicComp->getAngularFactor().x(), physicComp->getAngularFactor().y(), physicComp->getAngularFactor().z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Angular Factor", ImGuiDataType_Float, angularFactor)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setAngularFactor({val[0], val[1], val[2]});
        });

        std::array margin = {physicComp->getCollisionMargin()};
        m_physicGroup->createWidget<Drag<float>>("Margin", ImGuiDataType_Float, margin, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicObject>();
            physicComp->setCollisionMargin(val[0]);
        });
    }

    //! Draw PhysicBox component
    void Inspector::drawPhysicBox()
    {
        auto physicComp = m_targetObject->getComponent<PhysicBox>();
        if (physicComp == nullptr)
            return;

        drawPhysicObject();

        m_physicGroup->createWidget<Separator>();
        std::array size = {physicComp->getSize().X(), physicComp->getSize().Y(), physicComp->getSize().Z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Size", ImGuiDataType_Float, size, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBox>();
            physicComp->setSize({val[0], val[1], val[2]});
        });

        // Draw constraints
        drawPhysicConstraints();
    }

    //! Draw PhysicSphere component
    void Inspector::drawPhysicSphere()
    {
        auto physicComp = m_targetObject->getComponent<PhysicSphere>();
        if (physicComp == nullptr)
            return;

        drawPhysicObject();

        m_physicGroup->createWidget<Separator>();
        std::array radius = {physicComp->getRadius()};
        m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicSphere>();
            physicComp->setRadius(val[0]);
        });

        // Draw constraints
        drawPhysicConstraints();
    }

    //! Draw PhysicCapsule component
    void Inspector::drawPhysicCapsule()
    {
        auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
        if (physicComp == nullptr)
            return;

        drawPhysicObject();

        m_physicGroup->createWidget<Separator>();
        std::array height = {physicComp->getHeight()};
        m_physicGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
            physicComp->setHeight(val[0]);
        });
        std::array radius = {physicComp->getRadius()};
        m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
            physicComp->setRadius(val[0]);
        });

        // Draw constraints
        drawPhysicConstraints();
    }

    //! Draw PhysicMesh component
    void Inspector::drawPhysicMesh()
    {
        drawPhysicObject();

        auto physicComp = m_targetObject->getComponent<PhysicMesh>();
        if (physicComp == nullptr)
            return;

        m_physicGroup->createWidget<Separator>();

        bool convex = physicComp->isConvex();
        auto convexChk = m_physicGroup->createWidget<CheckBox>("Convex Hull", convex);
        convexChk->setEndOfLine(false);

        auto concaveChk = m_physicGroup->createWidget<CheckBox>("Triangle Mesh", !convex);
        convexChk->getOnDataChangedEvent().addListener([this, convexChk, concaveChk](bool convex) {
            auto physicComp = m_targetObject->getComponent<PhysicMesh>();
            physicComp->setConvex(convex);
            convexChk->setSelected(physicComp->isConvex());
            concaveChk->setSelected(!physicComp->isConvex());
        });

        concaveChk->getOnDataChangedEvent().addListener([this, convexChk, concaveChk](bool concave) {
            auto physicComp = m_targetObject->getComponent<PhysicMesh>();
            physicComp->setConvex(!concave);
            convexChk->setSelected(physicComp->isConvex());
            concaveChk->setSelected(!physicComp->isConvex());
        });

        std::array meshIdx = { physicComp->getMeshIndex() };
        auto meshIdxWg = m_physicGroup->createWidget<Drag<int>>("Mesh Index", ImGuiDataType_S32, meshIdx, 1, 0);
        meshIdxWg->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicMesh>();
            physicComp->setMeshIndex(val[0]);
        });
        meshIdxWg->addPlugin<DDTargetPlugin<int>>(EDragDropID::MESH)->getOnDataReceivedEvent().addListener([this](auto val) {
            auto physicComp = m_targetObject->getComponent<PhysicMesh>();
            physicComp->setMeshIndex(val);
            redraw();
        });

        auto txtPath = m_physicGroup->createWidget<TextField>("Path", physicComp->getPath());
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto physicComp = m_targetObject->getComponent<PhysicMesh>();
            physicComp->setPath(txt);
        });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto physicComp = m_targetObject->getComponent<PhysicMesh>();
                physicComp->setPath(txt);
                redraw();
            });
        }

        // Draw constraints
        drawPhysicConstraints();
    }

    void Inspector::drawPhysicSoftBody()
    {
        if (m_physicGroup == nullptr)
            return;
        m_physicGroup->removeAllWidgets();

        auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
        if (physicComp == nullptr)
            return;

        int maxMeshIdx = 0;
        auto figureComp = m_targetObject->getComponent<FigureComponent>();
        if (figureComp && figureComp->getFigure())
            maxMeshIdx = figureComp->getFigure()->NumMeshes() - 1;
        std::array meshIdx = { physicComp->getMeshIndex() };
        auto meshIdxWg = m_physicGroup->createWidget<Drag<int>>("Mesh Index", ImGuiDataType_S32, meshIdx, 1, 0, maxMeshIdx);
        meshIdxWg->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setMeshIndex(val[0]);
        });
        meshIdxWg->addPlugin<DDTargetPlugin<int>>(EDragDropID::MESH)->getOnDataReceivedEvent().addListener([this](auto val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setMeshIndex(val);
            redraw();
        });

        m_physicGroup->createWidget<Separator>();
        auto columns = m_physicGroup->createWidget<Columns<2>>();
        columns->createWidget<CheckBox>("Enable", physicComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setEnabled(val);
        });

        columns->createWidget<CheckBox>("Continous", physicComp->isCCD())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setCCD(val);
        });

        columns->createWidget<CheckBox>("Trigger", physicComp->isTrigger())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setIsTrigger(val);
        });

        columns->createWidget<CheckBox>("Self Collision", physicComp->isSelfCollision())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setSelfCollision(val);
        });

        columns->createWidget<CheckBox>("Soft Collision", physicComp->isSoftSoftCollision())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setSoftSoftCollision(val);
        });

        std::array filterGroup = { physicComp->getCollisionFilterGroup() };
        m_physicGroup->createWidget<Drag<int>>("Collision Group", ImGuiDataType_S32, filterGroup, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setCollisionFilterGroup(val[0]);
        });

        std::array filterMask = { physicComp->getCollisionFilterMask() };
        m_physicGroup->createWidget<Drag<int>>("Collision Mask", ImGuiDataType_S32, filterMask, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setCollisionFilterMask(val[0]);
        });

        std::array mass = { physicComp->getMass() };
        m_physicGroup->createWidget<Drag<float>>("Mass", ImGuiDataType_Float, mass, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setMass(val[0]);
        });

        std::array friction = { physicComp->getFriction() };
        m_physicGroup->createWidget<Drag<float>>("Friction", ImGuiDataType_Float, friction, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setFriction(val[0]);
        });

        std::array restitution = { physicComp->getRestitution() };
        m_physicGroup->createWidget<Drag<float>>("Restitution", ImGuiDataType_Float, restitution, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setRestitution(val[0]);
        });

        std::array linearVelocity = { physicComp->getLinearVelocity().x(), physicComp->getLinearVelocity().y(), physicComp->getLinearVelocity().z() };
        m_physicGroup->createWidget<Drag<float, 3>>("Linear Velocity", ImGuiDataType_Float, linearVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setLinearVelocity({ val[0], val[1], val[2] });
        });

        std::array angularVelocity = { physicComp->getAngularVelocity().x(), physicComp->getAngularVelocity().y(), physicComp->getAngularVelocity().z() };
        m_physicGroup->createWidget<Drag<float, 3>>("Angular Velocity", ImGuiDataType_Float, angularVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setAngularVelocity({ val[0], val[1], val[2] });
        });

        std::array margin = { physicComp->getCollisionMargin() };
        m_physicGroup->createWidget<Drag<float>>("Margin", ImGuiDataType_Float, margin, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setCollisionMargin(val[0]);
        });

        m_physicGroup->createWidget<Separator>();
        std::array damping = { physicComp->getDampingCoeff() };
        m_physicGroup->createWidget<Drag<float>>("DampingCoeff", ImGuiDataType_Float, damping, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setDampingCoeff(val[0]);
        });

        std::array spring = { physicComp->getRepulsionStiffness() };
        m_physicGroup->createWidget<Drag<float>>("LinearStiffness", ImGuiDataType_Float, spring, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setRepulsionStiffness(val[0]);
        });

        std::array pressure = { physicComp->getPressureCoeff() };
        m_physicGroup->createWidget<Drag<float>>("PressureCoeff", ImGuiDataType_Float, pressure, 0.001f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setPressureCoeff(val[0]);
        });

        std::array volCoeff = { physicComp->getVolumeConvCoeff() };
        m_physicGroup->createWidget<Drag<float>>("VolumeConvCoeff", ImGuiDataType_Float, volCoeff, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setVolumeConvCoeff(val[0]);
        });

        std::array fricCoeff = { physicComp->getDynamicFrictionCoeff() };
        m_physicGroup->createWidget<Drag<float>>("FrictionCoeff", ImGuiDataType_Float, fricCoeff, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setDynamicFrictionCoeff(val[0]);
        });

        std::array poseCoeff = { physicComp->getPoseMatchCoeff() };
        m_physicGroup->createWidget<Drag<float>>("PoseMatchCoeff", ImGuiDataType_Float, poseCoeff, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setPoseMatchCoeff(val[0]);
        });

        std::array graF = { physicComp->getGravityFactor() };
        m_physicGroup->createWidget<Drag<float>>("GravityFactor", ImGuiDataType_Float, graF, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setGravityFactor(val[0]);
        });

        std::array velF = { physicComp->getVelocityFactor() };
        m_physicGroup->createWidget<Drag<float>>("VelocityCorFactor", ImGuiDataType_Float, velF, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setVelocityFactor(val[0]);
        });

        std::array rigidH = { physicComp->getRigidContactHardness() };
        m_physicGroup->createWidget<Drag<float>>("RigidHardness", ImGuiDataType_Float, rigidH, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setRigidContactHardness(val[0]);
        });

        std::array kinH = { physicComp->getKineticContactHardness() };
        m_physicGroup->createWidget<Drag<float>>("KineticHardness", ImGuiDataType_Float, kinH, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setKineticContactHardness(val[0]);
        });

        std::array softH = { physicComp->getSoftContactHardness() };
        m_physicGroup->createWidget<Drag<float>>("SoftHardness", ImGuiDataType_Float, softH, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setSoftContactHardness(val[0]);
        });

        std::array anchorH = { physicComp->getAnchorHardness() };
        m_physicGroup->createWidget<Drag<float>>("AnchorHardness", ImGuiDataType_Float, anchorH, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setAnchorHardness(val[0]);
        });

        std::array posItrNum = { physicComp->getPosIterationNumber() };
        m_physicGroup->createWidget<Drag<int>>("PosIterations", ImGuiDataType_S32, posItrNum, 1, 1)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setPosIterationNumber(val[0]);
        });

        std::array sleepThreshold = { physicComp->getSleepingThreshold() };
        m_physicGroup->createWidget<Drag<float>>("SleepThreshold", ImGuiDataType_Float, sleepThreshold, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setSleepingThreshold(val[0]);
        });

        std::array restLS = { physicComp->getRestLengthScale() };
        m_physicGroup->createWidget<Drag<float>>("RestLengthScale", ImGuiDataType_Float, restLS, 0.001f, 0.0f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setRestLengthScale(val[0]);
        });

        std::array aeroModel = { physicComp->getAeroModel() };
        m_physicGroup->createWidget<Drag<int>>("AeroModel", ImGuiDataType_S32, aeroModel, 1, 0, 6)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setAeroModel(val[0]);
        });

        std::array windVelocity = { physicComp->getWindVelocity().x(), physicComp->getWindVelocity().y(), physicComp->getWindVelocity().z() };
        m_physicGroup->createWidget<Drag<float, 3>>("WindVelocity", ImGuiDataType_Float, windVelocity)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicSoftBody>();
            physicComp->setWindVelocity({ val[0], val[1], val[2] });
        });

        // Draw constraints
        drawPhysicConstraints();
    }

    //! Draw Physic Constraints
    void Inspector::drawPhysicConstraints()
    {
        m_constraintGroup = m_physicGroup->createWidget<Group>("ConstraintGroup");

        // Create constraint selection
        m_constraintCreateCombo = m_constraintGroup->createWidget<ComboBox>(0, true, false);
        m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Fixed, "Fixed Constraint");
        m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Hinge, "Hinge Constraint");
        m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Slider, "Slider Constraint");
        m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Spring, "Spring Constraint");
        m_constraintCreateCombo->addChoice((int)PhysicConstraint::ConstraintType::Dof6Spring, "Dof6-Spring Constraint");

        // Add button
        auto createCompButton = m_constraintGroup->createWidget<Button>("Add", ImVec2(64.f, 0.f));
        createCompButton->getOnClickEvent().addListener([this](auto widget) {
            switch (m_constraintCreateCombo->getSelectedIndex())
            {
                case (int)PhysicConstraint::ConstraintType::Fixed:
                    m_targetObject->getComponent<PhysicObject>()->addConstraint<FixedConstraint>();
                    break;
                case (int)PhysicConstraint::ConstraintType::Hinge:
                    m_targetObject->getComponent<PhysicObject>()->addConstraint<HingeConstraint>();
                    break;
                case (int)PhysicConstraint::ConstraintType::Slider:
                    m_targetObject->getComponent<PhysicObject>()->addConstraint<SliderConstraint>();
                    break;
                case (int)PhysicConstraint::ConstraintType::Spring:  
                    m_targetObject->getComponent<PhysicObject>()->addConstraint<SpringConstraint>();
                    break;
                case (int)PhysicConstraint::ConstraintType::Dof6Spring:
                    m_targetObject->getComponent<PhysicObject>()->addConstraint<Dof6SpringConstraint>();
                    break;
            }
            redraw();
        });
        m_constraintGroup->createWidget<Separator>();

        const auto& constraints = m_targetObject->getComponent<PhysicObject>()->getContraints();
        for (const auto& constraint : constraints)
        {
            if (constraint->getType() == PhysicConstraint::ConstraintType::Fixed)
            {
                drawFixedConstraint(constraint);
            }
            else if (constraint->getType() == PhysicConstraint::ConstraintType::Hinge)
            {
                drawHingeConstraint(constraint);
            }
            else if (constraint->getType() == PhysicConstraint::ConstraintType::Slider)
            {
                drawSliderConstraint(constraint);
            }
            else if (constraint->getType() == PhysicConstraint::ConstraintType::Spring)
            {
                drawSpringConstraint(constraint);
            }
            else if (constraint->getType() == PhysicConstraint::ConstraintType::Dof6Spring)
            {
                drawDof6SpringConstraint(constraint);
            }
        }
    }

    //! Draw Physic Constraint
    void Inspector::drawPhysicConstraint(const std::shared_ptr<PhysicConstraint>& constraint, std::shared_ptr<Group> constraintGroup)
    {
        auto columns = constraintGroup->createWidget<Columns<2>>();

        // Enabled
        columns->createWidget<CheckBox>("Enable", constraint->isEnabled())->getOnDataChangedEvent().addListener([&](bool val) {
            constraint->setEnabled(val);
        });        

        // Bodies Collision
        columns->createWidget<CheckBox>("Bodies Collision", constraint->isEnableCollisionBetweenBodies())->getOnDataChangedEvent().addListener([&](bool val) {
            constraint->setEnableCollisionBetweenBodies(val);
        });

        // Other body
        auto otherBodyTxt = constraintGroup->createWidget<TextField>("Other body", constraint->getOther() ? constraint->getOther()->getOwner()->getName() : std::string());
        otherBodyTxt->getOnDataChangedEvent().addListener([&](const auto& val) {
            auto obj = Editor::getCurrentScene()->findObjectByName(val);
            constraint->setOtherUUID(obj->getUUID());
        });
        otherBodyTxt->addPlugin<DDTargetPlugin<int>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([&](auto val) {
            auto obj = Editor::getCurrentScene()->findObjectById(val);
            constraint->setOtherUUID(obj->getUUID());
            redraw();
        });

        // Breaking impulse
        std::array breakImpulse = { constraint->getBreakingImpulseThreshold() };
        constraintGroup->createWidget<Drag<float>>("Break Impulse", ImGuiDataType_Float, breakImpulse)->getOnDataChangedEvent().addListener([&](const auto& val) { 
            constraint->setBreakingImpulseThreshold(val[0]);
        });
    }

    //! Draw Fixed Constraint
    void Inspector::drawFixedConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
    {
        auto constraintGroup = m_constraintGroup->createWidget<Group>("FixedConstraint", true, true);
        constraintGroup->getOnClosedEvent().addListener([&, this]() {
            m_targetObject->getComponent<PhysicObject>()->removeConstraint(constraint);
            redraw();
        });

        // Draw Physic Constraint base
        drawPhysicConstraint(constraint, constraintGroup);
    }

    //! draw Hinge Constraint
    void Inspector::drawHingeConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
    {
        auto constraintGroup = m_constraintGroup->createWidget<Group>("HingeConstraint", true, true);
        constraintGroup->getOnClosedEvent().addListener([&, this]() {
            m_targetObject->getComponent<PhysicObject>()->removeConstraint(constraint);
            redraw();
        });

        // Draw Physic Constraint base
        drawPhysicConstraint(constraint, constraintGroup);

        auto hingeConstraint = std::dynamic_pointer_cast<HingeConstraint>(constraint);
        if (hingeConstraint == nullptr)
            return;

        // Draw Hinge Constraint properties
        constraintGroup->createWidget<Separator>();

        // Anchor
        std::array anchor = { hingeConstraint->getAnchor().x(), hingeConstraint->getAnchor().y(), hingeConstraint->getAnchor().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Anchor", ImGuiDataType_Float, anchor)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
            hingeConstraint->setAnchor({ val[0], val[1], val[2] });
        });

        // Axis1
        std::array axis1 = { hingeConstraint->getAxis1().x(), hingeConstraint->getAxis1().y(), hingeConstraint->getAxis1().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Axis1", ImGuiDataType_Float, axis1)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
            hingeConstraint->setAxis1({ val[0], val[1], val[2] });
        });

        // Axis 2
        std::array axis2 = { hingeConstraint->getAxis2().x(), hingeConstraint->getAxis2().y(), hingeConstraint->getAxis2().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Axis2", ImGuiDataType_Float, axis2)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
            hingeConstraint->setAxis2({ val[0], val[1], val[2] });
        });

        // Lower limit
        std::array lowerLimit = { hingeConstraint->getLowerLimit() };
        constraintGroup->createWidget<Drag<float>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f, -SIMD_PI, SIMD_PI)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
            hingeConstraint->setLowerLimit(val[0]);
        });

        // Upper limit
        std::array upperLimit = { hingeConstraint->getUpperLimit() };
        constraintGroup->createWidget<Drag<float>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f, -SIMD_PI, SIMD_PI)->getOnDataChangedEvent().addListener([hingeConstraint](const auto& val) {
            hingeConstraint->setUpperLimit(val[0]);
        });
    }

    //! draw Slider Constraint
    void Inspector::drawSliderConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
    {
        auto constraintGroup = m_constraintGroup->createWidget<Group>("SliderConstraint", true, true);
        constraintGroup->getOnClosedEvent().addListener([&, this]() {
            m_targetObject->getComponent<PhysicObject>()->removeConstraint(constraint);
            redraw();
            });

        // Draw Physic Constraint base
        drawPhysicConstraint(constraint, constraintGroup);

        auto sliderConstraint = std::dynamic_pointer_cast<SliderConstraint>(constraint);
        if (sliderConstraint == nullptr)
            return;

        constraintGroup->createWidget<Separator>();

        // Lower limit
        std::array lowerLimit = { sliderConstraint->getLowerLimit().x(), sliderConstraint->getLowerLimit().y(), sliderConstraint->getLowerLimit().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f)->getOnDataChangedEvent().addListener([sliderConstraint](const auto& val) {
            sliderConstraint->setLowerLimit({ val[0], val[1], val[2] });
        });

        // Upper limit
        std::array upperLimit = { sliderConstraint->getUpperLimit().x(), sliderConstraint->getUpperLimit().y(), sliderConstraint->getUpperLimit().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f)->getOnDataChangedEvent().addListener([sliderConstraint](const auto& val) {
            sliderConstraint->setUpperLimit({ val[0], val[1], val[2] });
        });
    }

    //! draw Spring Constraint
    void Inspector::drawSpringConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
    {
        auto constraintGroup = m_constraintGroup->createWidget<Group>("SpringConstraint", true, true);
        constraintGroup->getOnClosedEvent().addListener([&, this]() {
            m_targetObject->getComponent<PhysicObject>()->removeConstraint(constraint);
            redraw();
            });

        // Draw Physic Constraint base
        drawPhysicConstraint(constraint, constraintGroup);

        auto springConstraint = std::dynamic_pointer_cast<SpringConstraint>(constraint);
        if (springConstraint == nullptr)
            return;

        constraintGroup->createWidget<Separator>();

        // Enable
        std::array enable = { springConstraint->getEnable().x(), springConstraint->getEnable().y(), springConstraint->getEnable().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Enable", ImGuiDataType_Float, enable, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
            springConstraint->setEnable({ val[0], val[1], val[2] });
        });

        // Stiffness
        std::array stiffness = { springConstraint->getStiffness().x(), springConstraint->getStiffness().y(), springConstraint->getStiffness().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Stiffness", ImGuiDataType_Float, stiffness, 0.001f, 0.f)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
            springConstraint->setStiffness({ val[0], val[1], val[2] });
        });

        // Damping
        std::array damping = { springConstraint->getDamping().x(), springConstraint->getDamping().y(), springConstraint->getDamping().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Damping", ImGuiDataType_Float, damping, 0.001f, 0.f)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
            springConstraint->setDamping({ val[0], val[1], val[2] });
        });

        // Lower limit
        std::array lowerLimit = { springConstraint->getLowerLimit().x(), springConstraint->getLowerLimit().y(), springConstraint->getLowerLimit().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f, -SIMD_PI, SIMD_PI)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
            springConstraint->setLowerLimit({ val[0], val[1], val[2] });
        });

        // Upper limit
        std::array upperLimit = { springConstraint->getUpperLimit().x(), springConstraint->getUpperLimit().y(), springConstraint->getUpperLimit().z() };
        constraintGroup->createWidget<Drag<float, 3>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f, -SIMD_PI, SIMD_PI)->getOnDataChangedEvent().addListener([springConstraint](const auto& val) {
            springConstraint->setUpperLimit({ val[0], val[1], val[2] });
        });
    }

    //! draw Dof6 Spring Constraint
    void Inspector::drawDof6SpringConstraint(const std::shared_ptr<PhysicConstraint>& constraint)
    {
        auto constraintGroup = m_constraintGroup->createWidget<Group>("Dof6Constraint", true, true);
        constraintGroup->getOnClosedEvent().addListener([&, this]() {
            m_targetObject->getComponent<PhysicObject>()->removeConstraint(constraint);
            redraw();
        });

        // Draw Physic Constraint base
        drawPhysicConstraint(constraint, constraintGroup);

        auto dof6Constraint = std::dynamic_pointer_cast<Dof6SpringConstraint>(constraint);
        if (dof6Constraint == nullptr)
            return;

        // Linear constraints
        auto linearGroup = constraintGroup->createWidget<Group>("Linear Constraints", true);
        {
            // Lower limit
            std::array lowerLimit = { dof6Constraint->getLinearLowerLimit().x(), dof6Constraint->getLinearLowerLimit().y(), dof6Constraint->getLinearLowerLimit().z() };
            linearGroup->createWidget<Drag<float, 3>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearLowerLimit({ val[0], val[1], val[2] });
            });

            // Upper limit
            std::array upperLimit = { dof6Constraint->getLinearUpperLimit().x(), dof6Constraint->getLinearUpperLimit().y(), dof6Constraint->getLinearUpperLimit().z() };
            linearGroup->createWidget<Drag<float, 3>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearUpperLimit({ val[0], val[1], val[2] });
            });

            // Velocity target
            std::array targetVelocity = { dof6Constraint->getLinearTargetVelocity().x(), dof6Constraint->getLinearTargetVelocity().y(), dof6Constraint->getLinearTargetVelocity().z() };
            linearGroup->createWidget<Drag<float, 3>>("Target Velocity", ImGuiDataType_Float, targetVelocity, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearTargetVelocity({ val[0], val[1], val[2] });
            });

            // Bounce
            std::array bounce = { dof6Constraint->getLinearBounce().x(), dof6Constraint->getLinearBounce().y(), dof6Constraint->getLinearBounce().z() };
            linearGroup->createWidget<Drag<float, 3>>("Bounce", ImGuiDataType_Float, bounce, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearBounce({ val[0], val[1], val[2] });
            });

            linearGroup->createWidget<Separator>();
            // Spring
            std::array spring = { dof6Constraint->getLinearSpringEnabled().x(), dof6Constraint->getLinearSpringEnabled().y(), dof6Constraint->getLinearSpringEnabled().z() };
            linearGroup->createWidget<Drag<float, 3>>("Enable Spring", ImGuiDataType_Float, spring, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearSpringEnabled({ val[0], val[1], val[2] });
            });

            // Stiffness
            std::array stiffness = { dof6Constraint->getLinearStiffness().x(), dof6Constraint->getLinearStiffness().y(), dof6Constraint->getLinearStiffness().z() };
            linearGroup->createWidget<Drag<float, 3>>("Stiffness", ImGuiDataType_Float, stiffness, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearStiffness({ val[0], val[1], val[2] });
            });

            // Damping
            std::array damping = { dof6Constraint->getLinearDamping().x(), dof6Constraint->getLinearDamping().y(), dof6Constraint->getLinearDamping().z() };
            linearGroup->createWidget<Drag<float, 3>>("Damping", ImGuiDataType_Float, damping, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearDamping({ val[0], val[1], val[2] });
            });

            linearGroup->createWidget<Separator>();
            // Motor
            std::array motor = { dof6Constraint->getLinearMotorEnabled().x(), dof6Constraint->getLinearMotorEnabled().y(), dof6Constraint->getLinearMotorEnabled().z() };
            linearGroup->createWidget<Drag<float, 3>>("Enable Motor", ImGuiDataType_Float, motor, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearMotorEnabled({ val[0], val[1], val[2] });
            });

            // Max motor force
            std::array motorMaxForce = { dof6Constraint->getLinearMaxMotorForce().x(), dof6Constraint->getLinearMaxMotorForce().y(), dof6Constraint->getLinearMaxMotorForce().z() };
            linearGroup->createWidget<Drag<float, 3>>("Max Motor Force", ImGuiDataType_Float, motorMaxForce, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearMaxMotorForce({ val[0], val[1], val[2] });
            });

            // Servo
            std::array servo = { dof6Constraint->getLinearServoEnabled().x(), dof6Constraint->getLinearServoEnabled().y(), dof6Constraint->getLinearServoEnabled().z() };
            linearGroup->createWidget<Drag<float, 3>>("Enable Servo", ImGuiDataType_Float, servo, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearServoEnabled({ val[0], val[1], val[2] });
            });

            // Servo target
            std::array servoTarget = { dof6Constraint->getLinearServoTarget().x(), dof6Constraint->getLinearServoTarget().y(), dof6Constraint->getLinearServoTarget().z() };
            linearGroup->createWidget<Drag<float, 3>>("Servo Target", ImGuiDataType_Float, servoTarget, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setLinearServoTarget({ val[0], val[1], val[2] });
            });
        }
        

        // Angular constraints
        auto angularGroup = constraintGroup->createWidget<Group>("Angular Constraints", true);
        {
            // Lower limit
            std::array lowerLimit = { dof6Constraint->getAngularLowerLimit().x(), dof6Constraint->getAngularLowerLimit().y(), dof6Constraint->getAngularLowerLimit().z() };
            angularGroup->createWidget<Drag<float, 3>>("Lower Limit", ImGuiDataType_Float, lowerLimit, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularLowerLimit({ val[0], val[1], val[2] });
            });

            // Upper limit
            std::array upperLimit = { dof6Constraint->getAngularUpperLimit().x(), dof6Constraint->getAngularUpperLimit().y(), dof6Constraint->getAngularUpperLimit().z() };
            angularGroup->createWidget<Drag<float, 3>>("Upper Limit", ImGuiDataType_Float, upperLimit, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularUpperLimit({ val[0], val[1], val[2] });
            });

            // Velocity target
            std::array targetVelocity = { dof6Constraint->getAngularTargetVelocity().x(), dof6Constraint->getAngularTargetVelocity().y(), dof6Constraint->getAngularTargetVelocity().z() };
            angularGroup->createWidget<Drag<float, 3>>("Target Velocity", ImGuiDataType_Float, targetVelocity, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularTargetVelocity({ val[0], val[1], val[2] });
            });

            // Bounce
            std::array bounce = { dof6Constraint->getAngularBounce().x(), dof6Constraint->getAngularBounce().y(), dof6Constraint->getAngularBounce().z() };
            angularGroup->createWidget<Drag<float, 3>>("Bounce", ImGuiDataType_Float, bounce, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularBounce({ val[0], val[1], val[2] });
            });

            angularGroup->createWidget<Separator>();
            // Spring
            std::array spring = { dof6Constraint->getAngularSpringEnabled().x(), dof6Constraint->getAngularSpringEnabled().y(), dof6Constraint->getAngularSpringEnabled().z() };
            angularGroup->createWidget<Drag<float, 3>>("Enable Spring", ImGuiDataType_Float, spring, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularSpringEnabled({ val[0], val[1], val[2] });
            });

            // Stiffness
            std::array stiffness = { dof6Constraint->getAngularStiffness().x(), dof6Constraint->getAngularStiffness().y(), dof6Constraint->getAngularStiffness().z() };
            angularGroup->createWidget<Drag<float, 3>>("Stiffness", ImGuiDataType_Float, stiffness, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularStiffness({ val[0], val[1], val[2] });
            });

            // Damping
            std::array damping = { dof6Constraint->getAngularDamping().x(), dof6Constraint->getAngularDamping().y(), dof6Constraint->getAngularDamping().z() };
            angularGroup->createWidget<Drag<float, 3>>("Damping", ImGuiDataType_Float, damping, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularDamping({ val[0], val[1], val[2] });
            });

            angularGroup->createWidget<Separator>();
            // Motor
            std::array motor = { dof6Constraint->getAngularMotorEnabled().x(), dof6Constraint->getAngularMotorEnabled().y(), dof6Constraint->getAngularMotorEnabled().z() };
            angularGroup->createWidget<Drag<float, 3>>("Enable Motor", ImGuiDataType_Float, motor, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularMotorEnabled({ val[0], val[1], val[2] });
            });

            // Max motor force
            std::array motorMaxForce = { dof6Constraint->getAngularMaxMotorForce().x(), dof6Constraint->getAngularMaxMotorForce().y(), dof6Constraint->getAngularMaxMotorForce().z() };
            angularGroup->createWidget<Drag<float, 3>>("Max Motor Force", ImGuiDataType_Float, motorMaxForce, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularMaxMotorForce({ val[0], val[1], val[2] });
            });

            // Servo
            std::array servo = { dof6Constraint->getAngularServoEnabled().x(), dof6Constraint->getAngularServoEnabled().y(), dof6Constraint->getAngularServoEnabled().z() };
            angularGroup->createWidget<Drag<float, 3>>("Enable Servo", ImGuiDataType_Float, servo, 1.f, 0.f, 1.f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularServoEnabled({ val[0], val[1], val[2] });
            });

            // Servo target
            std::array servoTarget = { dof6Constraint->getAngularServoTarget().x(), dof6Constraint->getAngularServoTarget().y(), dof6Constraint->getAngularServoTarget().z() };
            angularGroup->createWidget<Drag<float, 3>>("Servo Target", ImGuiDataType_Float, servoTarget, 0.001f)->getOnDataChangedEvent().addListener([dof6Constraint](const auto& val) {
                dof6Constraint->setAngularServoTarget({ val[0], val[1], val[2] });
            });
        }
    }

    //! Draw AudioSource
    void Inspector::drawAudioSource()
    {
        if (m_audioSourceGroup == nullptr)
            return;
        m_audioSourceGroup->removeAllWidgets();

        auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
        if (audioSourceComp == nullptr)
            return;

        auto columns = m_audioSourceGroup->createWidget<Columns<3>>();
        columns->createWidget<CheckBox>("Enable", audioSourceComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setEnabled(val);
        });
        columns->createWidget<CheckBox>("PlayOnEnable", audioSourceComp->getPlayOnEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setPlayOnEnabled(val);
        });
        columns->createWidget<Label>(""); // Empty node
        columns->createWidget<CheckBox>("Stream", audioSourceComp->isStream())->getOnDataChangedEvent().addListener([this](bool val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setStream(val);
        });
        columns->createWidget<CheckBox>("Loop", audioSourceComp->isLooped())->getOnDataChangedEvent().addListener([this](bool val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setLoop(val);
        });
        columns->createWidget<CheckBox>("Single", audioSourceComp->isSingleInstance())->getOnDataChangedEvent().addListener([this](bool val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setSingleInstance(val);
        });

        std::array volume = { audioSourceComp->getVolume() };
        m_audioSourceGroup->createWidget<Drag<float>>("Volume", ImGuiDataType_Float, volume, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setVolume(val[0]);
        });

        std::array pan = { audioSourceComp->getPan() };
        m_audioSourceGroup->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan, 0.01f, -1.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setPan(val[0]);
        });

        std::array minDistance = { audioSourceComp->getMinDistance() };
        m_audioSourceGroup->createWidget<Drag<float>>("Min Distance", ImGuiDataType_Float, minDistance, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setMinDistance(val[0]);
        });

        std::array maxDistance = { audioSourceComp->getMaxDistance() };
        m_audioSourceGroup->createWidget<Drag<float>>("Max Distance", ImGuiDataType_Float, maxDistance, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setMaxDistance(val[0]);
        });

        auto vel = audioSourceComp->getVelocity();
        std::array velocity = { vel[0], vel[1], vel[2] };
        m_audioSourceGroup->createWidget<Drag<float, 3>>("Velocity", ImGuiDataType_Float, velocity, 0.01f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setVelocity({ val[0], val[1], val[2] });
        });

        std::array attenuationModel = { audioSourceComp->getAttenuationModel() };
        m_audioSourceGroup->createWidget<Drag<int>>("Attenuation Model", ImGuiDataType_S32, attenuationModel, 1, 0, SoLoud::AudioSource::ATTENUATION_MODELS::EXPONENTIAL_DISTANCE)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setAttenuationModel(val[0]);
        });

        std::array attenuationFactor = { audioSourceComp->getAttenuationRollOffFactor() };
        m_audioSourceGroup->createWidget<Drag<float>>("Attenuation Factor", ImGuiDataType_Float, attenuationFactor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setAttenuationRollOffFactor(val[0]);
        });

        std::array dopplerFactor = { audioSourceComp->getDopplerFactor() };
        m_audioSourceGroup->createWidget<Drag<float>>("Doppler Factor", ImGuiDataType_Float, dopplerFactor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setDopplerFactor(val[0]);
        });

        auto txtPath = m_audioSourceGroup->createWidget<TextField>("Path", audioSourceComp->getPath());
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
            audioSourceComp->setPath(txt);
        });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Audio))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
                audioSourceComp->setPath(txt);
                redraw();
            });
        }
        m_audioSourceGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Audio (*.wav, *.ogg, *.mp3)", "*.wav", "*.ogg", "*.mp3" }).result();
            if (files.size() > 0)
            {
                auto audioSourceComp = m_targetObject->getComponent<AudioSource>();
                audioSourceComp->setPath(files[0]);
                redraw();
            }
        });
    }

    //! Draw AudioListener component
    void Inspector::drawAudioListener()
    {
        if (m_audioListenerGroup == nullptr)
            return;
        m_audioListenerGroup->removeAllWidgets();

        auto audioListenerComp = m_targetObject->getComponent<AudioListener>();
        if (audioListenerComp == nullptr)
            return;

        m_audioListenerGroup->createWidget<CheckBox>("Enable", audioListenerComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto audioListenerComp = m_targetObject->getComponent<AudioListener>();
            audioListenerComp->setEnabled(val);
        });
    }

    //! Draw AmbientLight
    void Inspector::drawAmbientLight()
    {
        if (m_ambientLightGroup == nullptr)
            return;
        m_ambientLightGroup->removeAllWidgets();

        auto ambientLight = m_targetObject->getComponent<AmbientLight>();
        if (ambientLight == nullptr)
            return;

        auto color = Vec4(ambientLight->getSkyColor(), 1.f);
        m_ambientLightGroup->createWidget<Color>("SkyColor", color)->getOnDataChangedEvent().addListener([this](auto val) {
            auto ambientLight = m_targetObject->getComponent<AmbientLight>();
            ambientLight->setSkyColor({ val[0], val[1], val[2] });
        });

        color = Vec4(ambientLight->getGroundColor(), 1.f);
        m_ambientLightGroup->createWidget<Color>("GroundColor", color)->getOnDataChangedEvent().addListener([this](auto val) {
            auto ambientLight = m_targetObject->getComponent<AmbientLight>();
            ambientLight->setGroundColor({ val[0], val[1], val[2] });
        });

        std::array direction = { ambientLight->getDirection().X(), ambientLight->getDirection().Y(), ambientLight->getDirection().Z() };
        m_ambientLightGroup->createWidget<Drag<float, 3>>("Direction", ImGuiDataType_Float, direction)->getOnDataChangedEvent().addListener([this](auto val) {
            auto ambientLight = m_targetObject->getComponent<AmbientLight>();
            ambientLight->setDirection({ val[0], val[1], val[2] });
        });
    }

    //! Draw DirectionalLight
    void Inspector::drawDirectionalLight()
    {
        if (m_directionalLightGroup == nullptr)
            return;
        m_directionalLightGroup->removeAllWidgets();

        auto directionalLight = m_targetObject->getComponent<DirectionalLight>();
        if (directionalLight == nullptr)
            return;

        auto color = Vec4(directionalLight->getColor(), 1.f);
        m_directionalLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
            auto directionalLight = m_targetObject->getComponent<DirectionalLight>();
            directionalLight->setColor({ val[0], val[1], val[2] });
        });

        std::array intensity = { directionalLight->getIntensity()};
        m_directionalLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, intensity, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto directionalLight = m_targetObject->getComponent<DirectionalLight>();
            directionalLight->setIntensity(val[0]);
        });
    }

    //! Draw PointLight
    void Inspector::drawPointLight()
    {
        if (m_pointLightGroup == nullptr)
            return;
        m_pointLightGroup->removeAllWidgets();

        auto pointLight = m_targetObject->getComponent<PointLight>();
        if (pointLight == nullptr)
            return;

        auto color = Vec4(pointLight->getColor(), 1.f);
        m_pointLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
            auto pointLight = m_targetObject->getComponent<PointLight>();
            pointLight->setColor({ val[0], val[1], val[2] });
        });

        std::array intensity = { pointLight->getIntensity() };
        m_pointLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, intensity, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto pointLight = m_targetObject->getComponent<PointLight>();
            pointLight->setIntensity(val[0]);
        });

        std::array range = { pointLight->getRange() };
        m_pointLightGroup->createWidget<Drag<float>>("Range", ImGuiDataType_Float, range, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto pointLight = m_targetObject->getComponent<PointLight>();
            pointLight->setRange(val[0]);
        });
    }
    
    //! Draw Spot Light
    void Inspector::drawSpotLight()
    {
        if (m_spotLightGroup == nullptr)
            return;
        m_spotLightGroup->removeAllWidgets();

        auto light = m_targetObject->getComponent<SpotLight>();
        if (light == nullptr)
            return;

        auto color = Vec4(light->getColor(), 1.f);
        m_spotLightGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
            auto light = m_targetObject->getComponent<SpotLight>();
            light->setColor({ val[0], val[1], val[2] });
        });

        std::array intensity = { light->getIntensity() };
        m_spotLightGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, intensity, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto light = m_targetObject->getComponent<SpotLight>();
            light->setIntensity(val[0]);
        });

        std::array range = { light->getRange() };
        m_spotLightGroup->createWidget<Drag<float>>("Range", ImGuiDataType_Float, range, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto light = m_targetObject->getComponent<SpotLight>();
            light->setRange(val[0]);
        });

        std::array angle = { light->getAngle() };
        m_spotLightGroup->createWidget<Drag<float>>("Angle", ImGuiDataType_Float, angle, 0.01f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto light = m_targetObject->getComponent<SpotLight>();
            light->setAngle(val[0]);
        });
    }
    
    //! Draw Particle
    void Inspector::drawParticle()
    {
        if (m_particleGroup == nullptr)
            return;
        m_particleGroup->removeAllWidgets();

        auto particle = m_targetObject->getComponent<Particle>();
        if (particle == nullptr)
            return;

        auto column = m_particleGroup->createWidget<Columns<3>>();
        column->createWidget<CheckBox>("Enable", particle->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setEnabled(val);
        });
        column->createWidget<CheckBox>("Loop", particle->isLooped())->getOnDataChangedEvent().addListener([this](bool val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setLoop(val);
        });
        column->createWidget<CheckBox>("Auto Drawing", particle->isAutoDrawing())->getOnDataChangedEvent().addListener([this](bool val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setAutoDrawing(val);
        });

        std::array layer = { particle->getLayer() };
        m_particleGroup->createWidget<Drag<int>>("Layer", ImGuiDataType_U32, layer, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setLayer(val[0]);
        });

        std::array mask = { particle->getGroupMask() };
        m_particleGroup->createWidget<Drag<int>>("Group Mask", ImGuiDataType_U32, mask, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setGroupMask(val[0]);
        });

        std::array speed = { particle->getSpeed() };
        m_particleGroup->createWidget<Drag<float>>("Speed", ImGuiDataType_Float, speed, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setSpeed(val[0]);
        });

        std::array timeScale = { particle->getTimeScale() };
        m_particleGroup->createWidget<Drag<float>>("Time Scale", ImGuiDataType_Float, timeScale, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setTimeScale(val[0]);
        });

        std::array target = { particle->getTargetLocation().X(), particle->getTargetLocation().Y(), particle->getTargetLocation().Z()};
        m_particleGroup->createWidget<Drag<float, 3>>("Target Location", ImGuiDataType_Float, target, 0.01f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setTargetLocation({ val[0], val[1], val[2] });
        });

        std::array params = { particle->getDynamicInputParameter().X(), particle->getDynamicInputParameter().Y(), particle->getDynamicInputParameter().Z(), particle->getDynamicInputParameter().W() };
        m_particleGroup->createWidget<Drag<float, 4>>("Dynamic Parameters", ImGuiDataType_Float, params, 0.01f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setDynamicInputParameter({ val[0], val[1], val[2], val[3] });
        });
        
        std::array color = { particle->getColor().X(), particle->getColor().Y(), particle->getColor().Z(), particle->getColor().W() };
        m_particleGroup->createWidget<Drag<float, 4>>("Color", ImGuiDataType_Float, color, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setColor({ val[0], val[1], val[2], val[3] });
        });

        auto txtPath = m_particleGroup->createWidget<TextField>("Path", particle->getPath());
        txtPath->getOnDataChangedEvent().addListener([this](const auto& val) {
            auto particle = m_targetObject->getComponent<Particle>();
            particle->setPath(val);
            particle->play();
        });
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Particle))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& val) {
                auto particle = m_targetObject->getComponent<Particle>();
                particle->setPath(val);
                particle->play();
                redraw();
            });
        }
    }

    //! Draw Navigable
    void Inspector::drawNavigable()
    {
        if (m_navigableGroup == nullptr)
            return;
        m_navigableGroup->removeAllWidgets();

        auto navigable = m_targetObject->getComponent<Navigable>();
        if (navigable == nullptr)
            return;

        auto column = m_navigableGroup->createWidget<Columns<3>>();
        column->createWidget<CheckBox>("Enable", navigable->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto navigable = m_targetObject->getComponent<Navigable>();
            navigable->setEnabled(val);
        });
        column->createWidget<CheckBox>("Recursive", navigable->isRecursive())->getOnDataChangedEvent().addListener([this](bool val) {
            auto navigable = m_targetObject->getComponent<Navigable>();
            navigable->setRecursive(val);
        });
    }

    //! Draw NavMesh
    void Inspector::drawNavMesh()
    {
        if (m_navMeshGroup == nullptr)
            return;
        m_navMeshGroup->removeAllWidgets();

        auto navMesh = m_targetObject->getComponent<NavMesh>();
        if (navMesh == nullptr)
            return;

        auto column = m_navMeshGroup->createWidget<Columns<3>>();
        column->createWidget<CheckBox>("Enable", navMesh->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setEnabled(val);
        });

        std::array tileSize = { navMesh->getTileSize() };
        m_navMeshGroup->createWidget<Drag<int>>("Tile Size", ImGuiDataType_S32, tileSize, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setTileSize(val[0]);
        });

        std::array cellSize = { navMesh->getCellSize() };
        m_navMeshGroup->createWidget<Drag<float>>("Cell Size", ImGuiDataType_Float, cellSize, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setCellSize(val[0]);
        });

        std::array cellHeight = { navMesh->getCellHeight() };
        m_navMeshGroup->createWidget<Drag<float>>("Cell Height", ImGuiDataType_Float, cellHeight, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setCellHeight(val[0]);
        });

        std::array agentRadius = { navMesh->getAgentRadius() };
        m_navMeshGroup->createWidget<Drag<float>>("Agent Radius", ImGuiDataType_Float, agentRadius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setAgentRadius(val[0]);
        });

        std::array agentHeight = { navMesh->getAgentHeight() };
        m_navMeshGroup->createWidget<Drag<float>>("Agent Height", ImGuiDataType_Float, agentHeight, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setAgentHeight(val[0]);
        });

        std::array agentMaxClimb = { navMesh->getAgentMaxClimb() };
        m_navMeshGroup->createWidget<Drag<float>>("Agent Max Climb", ImGuiDataType_Float, agentMaxClimb, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setAgentMaxClimb(val[0]);
        });

        std::array agentMaxSlope = { navMesh->getAgentMaxSlope() };
        m_navMeshGroup->createWidget<Drag<float>>("Agent Max Slope", ImGuiDataType_Float, agentMaxSlope, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setAgentMaxSlope(val[0]);
        });

        std::array regionMinSize = { navMesh->getRegionMinSize() };
        m_navMeshGroup->createWidget<Drag<float>>("Region Min Size", ImGuiDataType_Float, regionMinSize, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setRegionMinSize(val[0]);
        });

        std::array regionMergeSize = { navMesh->getRegionMergeSize() };
        m_navMeshGroup->createWidget<Drag<float>>("Region Merge Size", ImGuiDataType_Float, regionMergeSize, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setRegionMergeSize(val[0]);
        });

        std::array edgeMaxLength = { navMesh->getEdgeMaxLength() };
        m_navMeshGroup->createWidget<Drag<float>>("Edge Max Length", ImGuiDataType_Float, edgeMaxLength, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setEdgeMaxLength(val[0]);
        });

        std::array edgeMaxError = { navMesh->getEdgeMaxError() };
        m_navMeshGroup->createWidget<Drag<float>>("Edge Max Error", ImGuiDataType_Float, edgeMaxError, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setEdgeMaxError(val[0]);
        });

        std::array detailSampleDistance = { navMesh->getDetailSampleDistance() };
        m_navMeshGroup->createWidget<Drag<float>>("Detail Sample Distance", ImGuiDataType_Float, detailSampleDistance, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setDetailSampleDistance(val[0]);
        });

        std::array detailSampleMaxError = { navMesh->getDetailSampleMaxError() };
        m_navMeshGroup->createWidget<Drag<float>>("Detail Sample Max Error", ImGuiDataType_Float, detailSampleMaxError, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setDetailSampleMaxError(val[0]);
        });

        std::array padding = { navMesh->getPadding().X(), navMesh->getPadding().Y(), navMesh->getPadding().Z() };
        m_navMeshGroup->createWidget<Drag<float, 3>>("AABB Padding", ImGuiDataType_Float, padding, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setPadding({ val[0], val[1], val[2] });
        });

        std::array partitionType = { (int)navMesh->getPartitionType() };
        m_navMeshGroup->createWidget<Drag<int>>("Partition Type", ImGuiDataType_S32, partitionType, 1, (int)(NavMesh::EPartitionType::WATERSHED), (int)(NavMesh::EPartitionType::MONOTONE))->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<NavMesh>();
            navMesh->setPartitionType((NavMesh::EPartitionType)val[0]);
        });
    }

    //! Draw NavAgent
    void Inspector::drawNavAgent()
    {
        if (m_navAgentGroup == nullptr)
            return;
        m_navAgentGroup->removeAllWidgets();

        auto navAgent = m_targetObject->getComponent<NavAgent>();
        if (navAgent == nullptr)
            return;

        auto column = m_navAgentGroup->createWidget<Columns<3>>();
        column->createWidget<CheckBox>("Enable", navAgent->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setEnabled(val);
        });
        column->createWidget<CheckBox>("Sync Position", navAgent->isUpdateNodePosition())->getOnDataChangedEvent().addListener([this](bool val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setUpdateNodePosition(val);
        });

        std::array radius = { navAgent->getRadius() };
        m_navAgentGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setRadius(val[0]);
        });

        std::array height = { navAgent->getHeight() };
        m_navAgentGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setHeight(val[0]);
        });

        std::array maxAcceleration = { navAgent->getMaxAcceleration() };
        m_navAgentGroup->createWidget<Drag<float>>("Max Acceleration", ImGuiDataType_Float, maxAcceleration, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setMaxAcceleration(val[0]);
        });

        std::array maxSpeed = { navAgent->getMaxSpeed() };
        m_navAgentGroup->createWidget<Drag<float>>("Max Speed", ImGuiDataType_Float, maxSpeed, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setMaxSpeed(val[0]);
        });

        std::array targetPosition = { navAgent->getTargetPosition().X(), navAgent->getTargetPosition().Y(), navAgent->getTargetPosition().Z() };
        m_navAgentGroup->createWidget<Drag<float, 3>>("Target Position", ImGuiDataType_Float, targetPosition, 0.001f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setTargetPosition({ val[0], val[1], val[2] });
        });

        std::array queryFilterType = { (int)navAgent->getQueryFilterType() };
        m_navAgentGroup->createWidget<Drag<int>>("Query Filter Type", ImGuiDataType_S32, queryFilterType, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setQueryFilterType(val[0]);
        });

        std::array obstacleAvoidanceType = { (int)navAgent->getObstacleAvoidanceType() };
        m_navAgentGroup->createWidget<Drag<int>>("Obstacle Avoidance Type", ImGuiDataType_S32, obstacleAvoidanceType, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setObstacleAvoidanceType(val[0]);
        });

        std::array navigationQuality = { (int)navAgent->getNavigationQuality() };
        m_navAgentGroup->createWidget<Drag<int>>("Navigation Quality", ImGuiDataType_S32, navigationQuality, 1, (int)NavAgent::NavQuality::LOW, (int)NavAgent::NavQuality::HIGH)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setNavigationQuality((NavAgent::NavQuality)val[0]);
        });

        std::array navigationPushiness = { (int)navAgent->getNavigationPushiness() };
        m_navAgentGroup->createWidget<Drag<int>>("Navigation Pushiness", ImGuiDataType_S32, navigationPushiness, 1, (int)NavAgent::NavPushiness::LOW, (int)NavAgent::NavPushiness::HIGH)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgent = m_targetObject->getComponent<NavAgent>();
            navAgent->setNavigationPushiness((NavAgent::NavPushiness)val[0]);
        });
    }

    //! Draw NavAgentManager
    void Inspector::drawNavAgentManager()
    {
        if (m_navAgentManagerGroup == nullptr)
            return;
        m_navAgentManagerGroup->removeAllWidgets();

        auto navAgentManager = m_targetObject->getComponent<NavAgentManager>();
        if (navAgentManager == nullptr)
            return;

        std::array maxAgents = { (int)navAgentManager->getMaxAgentNumber() };
        m_navAgentManagerGroup->createWidget<Drag<int>>("Max Agents", ImGuiDataType_S32, maxAgents, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgentManager = m_targetObject->getComponent<NavAgentManager>();
            navAgentManager->setMaxAgentNumber(val[0]);
        });

        std::array maxRadius = { navAgentManager->getMaxAgentRadius() };
        m_navAgentManagerGroup->createWidget<Drag<float>>("Max Agent Radius", ImGuiDataType_Float, maxRadius, 0.001f, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navAgentManager = m_targetObject->getComponent<NavAgentManager>();
            navAgentManager->setMaxAgentRadius(val[0]);
        });
    }

    //! Draw DynamicNavMesh
    void Inspector::drawDynamicNavMesh()
    {
        drawNavMesh();

        auto navMesh = m_targetObject->getComponent<DynamicNavMesh>();
        if (navMesh == nullptr)
            return;

        std::array maxObstacles = { (int)navMesh->getMaxObstacles() };
        m_navMeshGroup->createWidget<Drag<int>>("Max Obstacles", ImGuiDataType_S32, maxObstacles, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<DynamicNavMesh>();
            navMesh->setMaxObstacles(val[0]);
        });

        std::array maxLayers = { (int)navMesh->getMaxLayers() };
        m_navMeshGroup->createWidget<Drag<int>>("Max Layers", ImGuiDataType_S32, maxLayers, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto navMesh = m_targetObject->getComponent<DynamicNavMesh>();
            navMesh->setMaxLayers(val[0]);
        });
    }

    //! Draw NavObstacle
    void Inspector::drawNavObstacle()
    {
        if (m_navObstacleGroup == nullptr)
            return;
        m_navObstacleGroup->removeAllWidgets();

        auto obstacle = m_targetObject->getComponent<NavObstacle>();
        if (obstacle == nullptr)
            return;

        auto column = m_navObstacleGroup->createWidget<Columns<3>>();
        column->createWidget<CheckBox>("Enable", obstacle->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto obstacle = m_targetObject->getComponent<NavObstacle>();
            obstacle->setEnabled(val);
        });

        std::array radius = { obstacle->getRadius() };
        m_navObstacleGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto obstacle = m_targetObject->getComponent<NavObstacle>();
            obstacle->setRadius(val[0]);
        });

        std::array height = { obstacle->getHeight() };
        m_navObstacleGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto obstacle = m_targetObject->getComponent<NavObstacle>();
            obstacle->setHeight(val[0]);
        });
    }

    //! Draw OffMeshLink
    void Inspector::drawOffMeshLink()
    {
        if (m_offMeshLinkGroup == nullptr)
            return;
        m_offMeshLinkGroup->removeAllWidgets();

        auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
        if (offMeshLink == nullptr)
            return;

        auto column = m_offMeshLinkGroup->createWidget<Columns<3>>();
        column->createWidget<CheckBox>("Enable", offMeshLink->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
            offMeshLink->setEnabled(val);
        });
        column->createWidget<CheckBox>("Bidirectional", offMeshLink->isBidirectional())->getOnDataChangedEvent().addListener([this](bool val) {
            auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
            offMeshLink->setBidirectional(val);
        });

        // Endpoint
        auto endPointTxt = m_offMeshLinkGroup->createWidget<TextField>("Endpoint", offMeshLink->getEndPoint() ? offMeshLink->getEndPoint()->getName() : std::string());
        endPointTxt->getOnDataChangedEvent().addListener([&](const auto& val) {
            auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
            auto obj = Editor::getCurrentScene()->findObjectByName(val);
            offMeshLink->setEndPoint(obj.get());
        });
        endPointTxt->addPlugin<DDTargetPlugin<int>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([&](auto val) {
            auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
            auto obj = Editor::getCurrentScene()->findObjectById(val);
            offMeshLink->setEndPoint(obj.get());
            redraw();
        });

        std::array radius = { offMeshLink->getRadius() };
        m_navObstacleGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
            offMeshLink->setRadius(val[0]);
        });

        std::array mask = { (int)offMeshLink->getMask() };
        m_navMeshGroup->createWidget<Drag<int>>("Mask", ImGuiDataType_S32, mask, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
            offMeshLink->setMask(val[0]);
        });
    
        std::array areaId = { (int)offMeshLink->getAreaId() };
        m_navMeshGroup->createWidget<Drag<int>>("Area ID", ImGuiDataType_S32, areaId, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
            offMeshLink->setAreaId(val[0]);
        });
    }

    void Inspector::_drawImpl()
    {
        if (m_bNeedRedraw)
        {
            initialize();
            m_bNeedRedraw = false;
            return;
        }

        Panel::_drawImpl();
    }

    void Inspector::clear()
    {
        if (m_headerGroup)
        {
            m_headerGroup->removeAllWidgets();
            m_headerGroup->removeAllPlugins();
            m_headerGroup = nullptr;
        }

        if (m_createCompCombo)
        {
            m_createCompCombo->removeAllPlugins();
            m_createCompCombo = nullptr;
        }

        if (m_componentGroup)
        {
            m_componentGroup->removeAllWidgets();
            m_componentGroup->removeAllPlugins();
            m_componentGroup = nullptr;
        }

        if (m_localTransformGroup)
        {
            m_localTransformGroup->removeAllWidgets();
            m_localTransformGroup->removeAllPlugins();
            m_localTransformGroup = nullptr;
        }

        if (m_worldTransformGroup)
        {
            m_worldTransformGroup->removeAllWidgets();
            m_worldTransformGroup->removeAllPlugins();
            m_worldTransformGroup = nullptr;
        }

        if (m_cameraCompGroup)
        {
            m_cameraCompGroup->removeAllWidgets();
            m_cameraCompGroup->removeAllPlugins();
            m_cameraCompGroup = nullptr;
        }

        if (m_cameraLockTargetGroup)
        {
            m_cameraLockTargetGroup->removeAllWidgets();
            m_cameraLockTargetGroup->removeAllPlugins();
            m_cameraLockTargetGroup = nullptr;
        }

        if (m_environmentCompGroup)
        {
            m_environmentCompGroup->removeAllWidgets();
            m_environmentCompGroup->removeAllPlugins();
            m_environmentCompGroup = nullptr;
        }

        if (m_figureCompGroup)
        {
            m_figureCompGroup->removeAllWidgets();
            m_figureCompGroup->removeAllPlugins();
            m_figureCompGroup = nullptr;
        }

        if (m_spriteCompGroup)
        {
            m_spriteCompGroup->removeAllWidgets();
            m_spriteCompGroup->removeAllPlugins();
            m_spriteCompGroup = nullptr;
        }

        if (m_rectTransformGroup)
        {
            m_rectTransformGroup->removeAllWidgets();
            m_rectTransformGroup->removeAllPlugins();
            m_rectTransformGroup = nullptr;
        }

        if (m_canvasGroup)
        {
            m_canvasGroup->removeAllWidgets();
            m_canvasGroup->removeAllPlugins();
            m_canvasGroup = nullptr;
        }

        if (m_uiImageGroup)
        {
            m_uiImageGroup->removeAllWidgets();
            m_uiImageGroup->removeAllPlugins();
            m_uiImageGroup = nullptr;
        }

        if (m_uiTextGroup)
        {
            m_uiTextGroup->removeAllWidgets();
            m_uiTextGroup->removeAllPlugins();
            m_uiTextGroup = nullptr;
        }

        if (m_physicGroup)
        {
            m_physicGroup->removeAllWidgets();
            m_physicGroup->removeAllPlugins();
            m_physicGroup = nullptr;
        }

        if (m_audioSourceGroup)
        {
            m_audioSourceGroup->removeAllWidgets();
            m_audioSourceGroup->removeAllPlugins();
            m_audioSourceGroup = nullptr;
        }

        if (m_audioListenerGroup)
        {
            m_audioListenerGroup->removeAllWidgets();
            m_audioListenerGroup->removeAllPlugins();
            m_audioListenerGroup = nullptr;
        }

        if (m_ambientLightGroup)
        {
            m_ambientLightGroup->removeAllWidgets();
            m_ambientLightGroup->removeAllPlugins();
            m_ambientLightGroup = nullptr;
        }

        if (m_directionalLightGroup)
        {
            m_directionalLightGroup->removeAllWidgets();
            m_directionalLightGroup->removeAllPlugins();
            m_directionalLightGroup = nullptr;
        }

        if (m_pointLightGroup)
        {
            m_pointLightGroup->removeAllWidgets();
            m_pointLightGroup->removeAllPlugins();
            m_pointLightGroup = nullptr;
        }

        if (m_spotLightGroup)
        {
            m_spotLightGroup->removeAllWidgets();
            m_spotLightGroup->removeAllPlugins();
            m_spotLightGroup = nullptr;
        }

        if (m_constraintGroup)
        {
            m_constraintGroup->removeAllWidgets();
            m_constraintGroup->removeAllPlugins();
            m_constraintGroup = nullptr;
        }

        if (m_constraintCreateCombo)
        {
            m_constraintCreateCombo->removeAllPlugins();
            m_constraintCreateCombo = nullptr;
        }

        if (m_particleGroup)
        {
            m_particleGroup->removeAllWidgets();
            m_particleGroup->removeAllPlugins();
            m_particleGroup = nullptr;
        }

        if (m_navigableGroup)
        {
            m_navigableGroup->removeAllWidgets();
            m_navigableGroup->removeAllPlugins();
            m_navigableGroup = nullptr;
        }

        if (m_navMeshGroup)
        {
            m_navMeshGroup->removeAllWidgets();
            m_navMeshGroup->removeAllPlugins();
            m_navMeshGroup = nullptr;
        }

        if (m_navAgentGroup)
        {
            m_navAgentGroup->removeAllWidgets();
            m_navAgentGroup->removeAllPlugins();
            m_navAgentGroup = nullptr;
        }

        if (m_navAgentManagerGroup)
        {
            m_navAgentManagerGroup->removeAllWidgets();
            m_navAgentManagerGroup->removeAllPlugins();
            m_navAgentManagerGroup = nullptr;
        }

        if (m_navObstacleGroup)
        {
            m_navObstacleGroup->removeAllWidgets();
            m_navObstacleGroup->removeAllPlugins();
            m_navObstacleGroup = nullptr;
        }

        if (m_offMeshLinkGroup)
        {
            m_offMeshLinkGroup->removeAllWidgets();
            m_offMeshLinkGroup->removeAllPlugins();
            m_offMeshLinkGroup = nullptr;
        }

        removeAllWidgets();
    }

    void Inspector::setTargetObject(const std::shared_ptr<SceneObject> &obj)
    {
        if (m_targetObject != obj)
        {
            if(m_targetObject)
                m_targetObject->getTransformChangedEvent().removeAllListeners();
            m_targetObject = obj;

            clear();

            if (m_targetObject != nullptr)
            {
                m_targetObject->getTransformChangedEvent().addListener(std::bind(&Inspector::onTransformChanged, this, std::placeholders::_1));
                initialize();
            }
        }
    }

    void Inspector::onTransformChanged(SceneObject& sceneObject)
    {
        // Just redraw the transform in Inspector
        TaskManager::getInstance()->addTask([this](){
            if (m_targetObject != nullptr)
            {
                if (m_targetObject->isGUIObject())
                {
                    drawRectTransform();
                }
                else
                {
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                }
            }
        });
    }

    void Inspector::updateMaterial(int index, const char *infoName, std::string txt)
    {
        auto figureComp = m_targetObject->getComponent<FigureComponent>();
        auto figure = figureComp->getFigure();
        if (figure)
        {
            Sampler sampler;
            auto tex = ResourceCreator::Instance().NewTexture(txt.c_str());
            sampler.tex = tex;
            sampler.samplerSlotNo = 0;
            if (tex)
            {
                figure->SetMaterialParam(index, infoName, &sampler);
            }
        }
    }
} // namespace ige::creator
