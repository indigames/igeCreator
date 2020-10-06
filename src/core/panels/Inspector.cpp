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

#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"

#include <components/CameraComponent.h>
#include <components/TransformComponent.h>
#include <components/EnvironmentComponent.h>
#include <components/FigureComponent.h>
#include <components/SpriteComponent.h>
#include <components/ScriptComponent.h>
#include <components/gui/RectTransform.h>
#include <components/gui/Canvas.h>
#include <components/gui/UIImage.h>
#include <components/gui/UIText.h>
#include <components/gui/UITextField.h>
#include <components/physic/PhysicBase.h>
#include <components/physic/PhysicBox.h>
#include <components/physic/PhysicSphere.h>
#include <components/physic/PhysicCapsule.h>
#include <components/audio/AudioSource.h>
#include <components/audio/AudioListener.h>
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
        UIImage,
        UIText,
        UITextField,
        PhysicBox,
        PhysicSphere,
        PhysicCapsule,
        AudioSource,
        AudioListener
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
        m_createCompCombo->addChoice((int)ComponentType::Camera, "Camera Component");
        m_createCompCombo->addChoice((int)ComponentType::Environment, "Environment Component");

        // Scene Object
        if (!m_targetObject->isGUIObject())
        {
            m_createCompCombo->addChoice((int)ComponentType::Figure, "Figure Component");
            m_createCompCombo->addChoice((int)ComponentType::Sprite, "Sprite Component");

            if (m_targetObject->getComponent<PhysicBase>() == nullptr)
            {
                m_createCompCombo->addChoice((int)ComponentType::PhysicBox, "PhysicBox");
                m_createCompCombo->addChoice((int)ComponentType::PhysicSphere, "PhysicSphere");
                m_createCompCombo->addChoice((int)ComponentType::PhysicCapsule, "PhysicCapsule");
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

        auto createCompButton = m_headerGroup->createWidget<Button>("Add", ImVec2(64.f, 0.f));
        createCompButton->getOnClickEvent().addListener([this](auto widget) {
            switch (m_createCompCombo->getSelectedIndex())
            {
            case (int)ComponentType::Camera:
                m_targetObject->addComponent<CameraComponent>("camera");
                break;
            case (int)ComponentType::Environment:
                m_targetObject->addComponent<EnvironmentComponent>("environment");
                break;
            case (int)ComponentType::Script:
                m_targetObject->addComponent<ScriptComponent>();
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
            case (int)ComponentType::AudioSource:
                m_targetObject->addComponent<AudioSource>();
                break;
            case (int)ComponentType::AudioListener:
                m_targetObject->addComponent<AudioListener>();
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
        });
    }

    void Inspector::drawLocalTransformComponent()
    {
        m_localTransformGroup->removeAllWidgets();
        auto transform = m_targetObject->getTransform();
        if (transform == nullptr)
            return;

        m_localTransformGroup->createWidget<Label>("Local");

        std::array pos = {transform->getPosition().X(), transform->getPosition().Y(), transform->getPosition().Z()};
        m_localTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            transform->setPosition({val[0], val[1], val[2]});
            transform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

        Vec3 euler;
        vmath_quatToEuler(transform->getRotation().P(), euler.P());
        std::array rot = {RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z())};
        m_localTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
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
            auto transform = m_targetObject->getTransform();
            transform->setScale({val[0], val[1], val[2]});
            transform->onUpdate(0.f);
            drawWorldTransformComponent();
        });
    }

    void Inspector::drawWorldTransformComponent()
    {
        m_worldTransformGroup->removeAllWidgets();
        auto transform = m_targetObject->getTransform();
        if (transform == nullptr)
            return;

        m_worldTransformGroup->createWidget<Label>("World");
        std::array pos = {transform->getWorldPosition().X(), transform->getWorldPosition().Y(), transform->getWorldPosition().Z()};
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            transform->setWorldPosition({val[0], val[1], val[2]});
            drawLocalTransformComponent();
        });

        Vec3 euler;
        vmath_quatToEuler(transform->getWorldRotation().P(), euler.P());
        std::array rot = {RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z())};
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            Quat quat;
            float rad[3] = {DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2])};
            vmath_eulerToQuat(rad, quat.P());
            transform->setWorldRotation(quat);
            drawLocalTransformComponent();
        });

        std::array scale = {transform->getWorldScale().X(), transform->getWorldScale().Y(), transform->getWorldScale().Z()};
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            transform->setWorldScale({val[0], val[1], val[2]});
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

        // Ambient
        auto ambientGroup = m_environmentCompGroup->createWidget<Group>("AmbientLight");
        std::array ambientSkyColor = {environment->getAmbientSkyColor().X(), environment->getAmbientSkyColor().Y(), environment->getAmbientSkyColor().Z()};
        ambientGroup->createWidget<Drag<float, 3>>("SkyColor", ImGuiDataType_Float, ambientSkyColor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setAmbientSkyColor({val[0], val[1], val[2]});
        });
        std::array ambientGroundColor = {environment->getAmbientGroundColor().X(), environment->getAmbientGroundColor().Y(), environment->getAmbientGroundColor().Z()};
        ambientGroup->createWidget<Drag<float, 3>>("GroundColor", ImGuiDataType_Float, ambientGroundColor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setAmbientGroundColor({val[0], val[1], val[2]});
        });
        std::array ambientDir = {environment->getAmbientDirection().X(), environment->getAmbientDirection().Y(), environment->getAmbientDirection().Z()};
        ambientGroup->createWidget<Drag<float, 3>>("Direction", ImGuiDataType_Float, ambientDir)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setAmbientDirection({val[0], val[1], val[2]});
        });

        // Directional
        auto directionalGroup = m_environmentCompGroup->createWidget<Group>("DirectionalLight");
        for (int i = 0; i < 3; i++)
        {
            directionalGroup->createWidget<Label>("Light " + std::to_string(i));
            std::array directCol = {environment->getDirectionalLightColor(i).X(), environment->getDirectionalLightColor(i).Y(), environment->getDirectionalLightColor(i).Z()};
            directionalGroup->createWidget<Drag<float, 3>>("Color", ImGuiDataType_Float, directCol, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setDirectionalLightColor(i, {val[0], val[1], val[2]});
            });
            std::array dir = {environment->getDirectionalLightDirection(i).X(), environment->getDirectionalLightDirection(i).Y(), environment->getDirectionalLightDirection(i).Z()};
            directionalGroup->createWidget<Drag<float, 3>>("Direction", ImGuiDataType_Float, dir)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setDirectionalLightDirection(i, {val[0], val[1], val[2]});
            });
            std::array intensity = {environment->getDirectionalLightIntensity(i)};
            directionalGroup->createWidget<Drag<float>>("Intensity", ImGuiDataType_Float, intensity)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setDirectionalLightIntensity(i, val[0]);
            });
        }

        // Point
        auto pointGroup = m_environmentCompGroup->createWidget<Group>("PointLight");
        for (int i = 0; i < 7; i++)
        {
            pointGroup->createWidget<Label>("Light " + std::to_string(i));
            std::array color = {environment->getPointLightColor(i).X(), environment->getPointLightColor(i).Y(), environment->getPointLightColor(i).Z()};
            pointGroup->createWidget<Drag<float, 3>>("Color", ImGuiDataType_Float, color, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setPointLightColor(i, {val[0], val[1], val[2]});
            });
            std::array pos = {environment->getPointLightPosition(i).X(), environment->getPointLightPosition(i).Y(), environment->getPointLightPosition(i).Z()};
            pointGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setPointLightPosition(i, {val[0], val[1], val[2]});
            });

            auto col2 = pointGroup->createWidget<Columns<2>>(140.f);
            std::array intensity = {environment->getPointLightIntensity(i)};
            col2->createWidget<Drag<float>>("Int.", ImGuiDataType_Float, intensity)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setPointLightIntensity(i, val[0]);
            });
            std::array range = {environment->getPointLightRange(i)};
            col2->createWidget<Drag<float>>("Range", ImGuiDataType_Float, range)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setPointLightRange(i, val[0]);
            });
        }

        // Shadow
        auto shadowGroup = m_environmentCompGroup->createWidget<Group>("Shadow");
        std::array shadowColor = {environment->getShadowColor().X(), environment->getShadowColor().Y(), environment->getShadowColor().Z()};
        shadowGroup->createWidget<Drag<float, 3>>("Color", ImGuiDataType_Float, shadowColor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowColor({val[0], val[1], val[2]});
        });
        auto shadowColumn = shadowGroup->createWidget<Columns<2>>(140.f);
        std::array density = {environment->getShadowDensity()};
        shadowColumn->createWidget<Drag<float>>("Density", ImGuiDataType_Float, density, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowDensity(val[0]);
        });
        std::array wideness = {environment->getShadowWideness()};
        shadowColumn->createWidget<Drag<float>>("Wideness", ImGuiDataType_Float, wideness, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowWideness(val[0]);
        });

        // Fog
        auto fogGroup = m_environmentCompGroup->createWidget<Group>("Fog");
        std::array fogColor = {environment->getDistanceFogColor().X(), environment->getDistanceFogColor().Y(), environment->getDistanceFogColor().Z()};
        fogGroup->createWidget<Drag<float, 3>>("Color", ImGuiDataType_Float, fogColor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogColor({val[0], val[1], val[2]});
        });
        auto fogColumn = fogGroup->createWidget<Columns<3>>(120.f);
        std::array fogNear = {environment->getDistanceFogNear()};
        fogColumn->createWidget<Drag<float>>("Near", ImGuiDataType_Float, fogNear, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogNear(val[0]);
        });
        std::array fogFar = {environment->getDistanceFogFar()};
        fogColumn->createWidget<Drag<float>>("Far", ImGuiDataType_Float, fogNear, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogFar(val[0]);
        });
        std::array fogAlpha = {environment->getDistanceFogAlpha()};
        fogColumn->createWidget<Drag<float>>("Alpha", ImGuiDataType_Float, fogAlpha, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogAlpha(val[0]);
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

        auto txtPath = m_figureCompGroup->createWidget<TextField>("Path", figureComp->getPath().c_str(), true);
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

        auto figure = figureComp->getFigure();
        if (figure)
        {
            for (int i = 0; i < figure->NumMaterials(); i++)
            {
                const char *matName = figure->GetMaterialName(i);
                if (matName)
                {
                    m_figureCompGroup->createWidget<Label>(matName);
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
                            m_figureCompGroup->createWidget<Color>(info->name, currMat->params[j].fValue);
                        }
                        else if ((currMat->params[j].type == ParamTypeSampler))
                        {
                            auto textPath = currMat->params[j].sampler.tex->ResourceName();
                            auto txtPath = m_figureCompGroup->createWidget<TextField>(info->name, textPath, true);
                            txtPath->getOnDataChangedEvent().addListener([this, index, infoName](auto txt) {
                                updateMaterial(index, infoName, txt);
                            });

                            for (const auto &type : GetFileExtensionSuported(E_FileExts::Sprite))
                            {
                                txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this, index, infoName](auto txt) {
                                    updateMaterial(index, infoName, txt);
                                    redraw();
                                });
                            }
                        }
                        else if ((currMat->params[j].type == ParamTypeFloat))
                        {
                            std::array val = {currMat->params[j].fValue[0]};
                            m_figureCompGroup->createWidget<Drag<float>>(info->name, ImGuiDataType_Float, val)->getOnDataChangedEvent().addListener([this, index, infoName](auto val) {
                                auto figureComp = m_targetObject->getComponent<FigureComponent>();
                                auto figure = figureComp->getFigure();
                                if (figure)
                                {
                                    figure->SetMaterialParam(index, infoName, &val);
                                }
                            });
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

        auto txtPath = m_spriteCompGroup->createWidget<TextField>("Path", spriteComp->getPath().c_str(), true);
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
        m_spriteCompGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, size, 1.f, 0.f, 4096.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setSize({val[0], val[1]});
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

        auto txtPath = m_scriptCompGroup->createWidget<TextField>("Path", scriptComp->getPath().c_str(), true);
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

    void Inspector::drawRectTransform()
    {
        if (m_rectTransformGroup == nullptr)
            return;
        m_rectTransformGroup->removeAllWidgets();

        auto rectTransform = m_targetObject->getComponent<RectTransform>();
        if (rectTransform == nullptr)
            return;

        m_rectTransformGroup->createWidget<AnchorPresets>("AnchorPresets")->getOnClickEvent().addListener([this](const auto &widget) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            auto anchor = rectTransform->getAnchor();
            auto anchorWidget = (AnchorWidget *)widget;
            anchor[0] = anchorWidget->getAnchorMin().x;
            anchor[1] = anchorWidget->getAnchorMin().y;
            anchor[2] = anchorWidget->getAnchorMax().x;
            anchor[3] = anchorWidget->getAnchorMax().y;
            rectTransform->setAnchor(anchor);
            redraw();
        });

        auto anchorColumn = m_rectTransformGroup->createWidget<Columns<2>>();
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
            std::array left = {offset[0]};
            anchorGroupColums->createWidget<Drag<float>>("L", ImGuiDataType_Float, left, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[0] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array top = {offset[1]};
            anchorGroupColums->createWidget<Drag<float>>("T", ImGuiDataType_Float, top, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[1] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = {rectTransform->getPosition().Z()};
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array right = {offset[2]};
            anchorGroupColums->createWidget<Drag<float>>("R", ImGuiDataType_Float, right, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[2] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array bottom = {offset[3]};
            anchorGroupColums->createWidget<Drag<float>>("B", ImGuiDataType_Float, bottom, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[3] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });
        }
        else if (anchor[0] == 0.f && anchor[2] == 1.f)
        {
            std::array left = {offset[0]};
            anchorGroupColums->createWidget<Drag<float>>("L", ImGuiDataType_Float, left, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[0] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });
            std::array posY = {rectTransform->getPosition().Y()};
            anchorGroupColums->createWidget<Drag<float>>("Y", ImGuiDataType_Float, posY, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Y(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });
            std::array posZ = {rectTransform->getPosition().Z()};
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array right = {offset[2]};
            anchorGroupColums->createWidget<Drag<float>>("R", ImGuiDataType_Float, right, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[2] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array height = {rectTransform->getSize().Y()};
            anchorGroupColums->createWidget<Drag<float>>("H", ImGuiDataType_Float, height, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.Y(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });
        }
        else if (anchor[1] == 0.f && anchor[3] == 1.f)
        {
            std::array posX = {rectTransform->getPosition().X()};
            anchorGroupColums->createWidget<Drag<float>>("X", ImGuiDataType_Float, posX, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.X(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array top = {offset[1]};
            anchorGroupColums->createWidget<Drag<float>>("T", ImGuiDataType_Float, top, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[1] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = {rectTransform->getPosition().Z()};
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array width = {rectTransform->getSize().X()};
            anchorGroupColums->createWidget<Drag<float>>("W", ImGuiDataType_Float, width, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.X(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });

            std::array bottom = {offset[3]};
            anchorGroupColums->createWidget<Drag<float>>("B", ImGuiDataType_Float, bottom, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset[3] = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });
        }
        else
        {
            std::array posX = {rectTransform->getPosition().X()};
            anchorGroupColums->createWidget<Drag<float>>("X", ImGuiDataType_Float, posX, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.X(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array posY = {rectTransform->getPosition().Y()};
            anchorGroupColums->createWidget<Drag<float>>("Y", ImGuiDataType_Float, posY, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Y(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = {rectTransform->getPosition().Z()};
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array width = {rectTransform->getSize().X()};
            anchorGroupColums->createWidget<Drag<float>>("W", ImGuiDataType_Float, width, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.X(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });

            std::array height = {rectTransform->getSize().Y()};
            anchorGroupColums->createWidget<Drag<float>>("H", ImGuiDataType_Float, height, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.Y(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });
        }

        std::array pivot = {rectTransform->getPivot().X(), rectTransform->getPivot().Y()};
        m_rectTransformGroup->createWidget<Drag<float, 2>>("Pivot", ImGuiDataType_Float, pivot, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            rectTransform->setPivot({val[0], val[1]});
            rectTransform->onUpdate(0.f);
        });

        Vec3 euler;
        vmath_quatToEuler(rectTransform->getRotation().P(), euler.P());
        std::array rot = {RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z())};
        m_rectTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            Quat quat;
            float rad[3] = {DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2])};
            vmath_eulerToQuat(rad, quat.P());
            rectTransform->setRotation(quat);
            rectTransform->onUpdate(0.f);
        });

        std::array scale = {rectTransform->getScale().X(), rectTransform->getScale().Y(), rectTransform->getScale().Z()};
        m_rectTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            rectTransform->setScale({val[0], val[1], val[2]});
            rectTransform->onUpdate(0.f);
        });
    }

    void Inspector::drawCanvas()
    {
        if (m_canvasGroup == nullptr)
            return;
        m_canvasGroup->removeAllWidgets();

        auto canvas = m_targetObject->getComponent<ige::scene::Canvas>();
        if (canvas == nullptr)
            return;

        std::array size = {canvas->getDesignCanvasSize().X(), canvas->getDesignCanvasSize().Y()};
        m_canvasGroup->createWidget<Drag<float, 2>>("Design Size", ImGuiDataType_Float, size)->getOnDataChangedEvent().addListener([this](auto val) {
            auto canvas = m_targetObject->getComponent<ige::scene::Canvas>();
            canvas->setDesignCanvasSize({val[0], val[1]});
        });

        std::array targetSize = {canvas->getTargetCanvasSize().X(), canvas->getTargetCanvasSize().Y()};
        m_canvasGroup->createWidget<Drag<float, 2>>("Target Size", ImGuiDataType_Float, targetSize)->getOnDataChangedEvent().addListener([this](auto val) {
            auto canvas = m_targetObject->getComponent<ige::scene::Canvas>();
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

        auto txtPath = m_uiImageGroup->createWidget<TextField>("Path", uiImage->getPath().c_str(), true);
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

        auto txtText = m_uiTextGroup->createWidget<TextField>("Text", uiText->getText().c_str(), true);
        txtText->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setText(txt);
        });

        auto txtFontPath = m_uiTextGroup->createWidget<TextField>("Font", uiText->getFontPath().c_str(), true);
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
        m_uiTextGroup->createWidget<Drag<float>>("Size", ImGuiDataType_Float, size, 1.f, 4.f, 50.f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setFontSize((int)val[0]);
        });

        static Vec4 color4;
        color4 = uiText->getColor();
        auto color = m_uiTextGroup->createWidget<Color>("Color", color4.P());
        color->getOnDataChangedEvent().addListener([this](auto &color) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setColor({color[0], color[1], color[2], color[3]});
        });
    }

    //! Draw PhysicBase component
    void Inspector::drawPhysicBase()
    {
        if (m_physicGroup == nullptr)
            return;
        m_physicGroup->removeAllWidgets();

        auto physicComp = m_targetObject->getComponent<PhysicBase>();
        if (physicComp == nullptr)
            return;

        auto columns = m_physicGroup->createWidget<Columns<2>>();
         columns->createWidget<CheckBox>("Enable", physicComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setEnabled(val);
        });

        columns->createWidget<CheckBox>("Continous", physicComp->isCCD())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setCCD(val);
        });

        columns->createWidget<CheckBox>("Kinematic", physicComp->isKinematic())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setIsKinematic(val);
        });

        columns->createWidget<CheckBox>("Trigger", physicComp->isTrigger())->getOnDataChangedEvent().addListener([this](bool val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setIsTrigger(val);
        });

        std::array filterGroup = { physicComp->getCollisionFilterGroup() };
        m_physicGroup->createWidget<Drag<int>>("Collision Group", ImGuiDataType_S32, filterGroup, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setCollisionFilterGroup(val[0]);
        });

        std::array filterMask = { physicComp->getCollisionFilterMask() };
        m_physicGroup->createWidget<Drag<int>>("Collision Mask", ImGuiDataType_S32, filterMask, 1, -1)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setCollisionFilterMask(val[0]);
        });

        std::array mass = {physicComp->getMass()};
        m_physicGroup->createWidget<Drag<float>>("Mass", ImGuiDataType_Float, mass, 0.01f, 0.01f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setMass(val[0]);
        });

        std::array friction = {physicComp->getFriction()};
        m_physicGroup->createWidget<Drag<float>>("Friction", ImGuiDataType_Float, friction, 0.01f, 0.01f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setFriction(val[0]);
        });

        std::array restitution = {physicComp->getRestitution()};
        m_physicGroup->createWidget<Drag<float>>("Restitution", ImGuiDataType_Float, restitution, 0.01f, 0.01f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setRestitution(val[0]);
        });

        std::array linearVelocity = {physicComp->getLinearVelocity().x(), physicComp->getLinearVelocity().y(), physicComp->getLinearVelocity().z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Linear Velocity", ImGuiDataType_Float, linearVelocity)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setLinearVelocity({val[0], val[1], val[2]});
        });

        std::array angularVelocity = {physicComp->getAngularVelocity().x(), physicComp->getAngularVelocity().y(), physicComp->getAngularVelocity().z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Angular Velocity", ImGuiDataType_Float, angularVelocity)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setAngularVelocity({val[0], val[1], val[2]});
        });

        std::array linearFactor = {physicComp->getLinearFactor().x(), physicComp->getLinearFactor().y(), physicComp->getLinearFactor().z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Linear Factor", ImGuiDataType_Float, linearFactor)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setLinearFactor({val[0], val[1], val[2]});
        });

        std::array angularFactor = {physicComp->getAngularFactor().x(), physicComp->getAngularFactor().y(), physicComp->getAngularFactor().z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Angular Factor", ImGuiDataType_Float, angularFactor)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setAngularFactor({val[0], val[1], val[2]});
        });

        std::array posOffset = {physicComp->getPositionOffset()[0], physicComp->getPositionOffset()[1], physicComp->getPositionOffset()[2]};
        m_physicGroup->createWidget<Drag<float, 3>>("Position Offset", ImGuiDataType_Float, posOffset)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBase>();
            physicComp->setPositionOffset({val[0], val[1], val[2]});
        });
    }

    //! Draw PhysicBox component
    void Inspector::drawPhysicBox()
    {
        auto physicComp = m_targetObject->getComponent<PhysicBox>();
        if (physicComp == nullptr)
            return;

        drawPhysicBase();

        m_physicGroup->createWidget<Separator>();
        std::array size = {physicComp->getSize().X(), physicComp->getSize().Y(), physicComp->getSize().Z()};
        m_physicGroup->createWidget<Drag<float, 3>>("Size", ImGuiDataType_Float, size, 0.01f, 0.01f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicBox>();
            physicComp->setSize({val[0], val[1], val[2]});
        });
    }

    //! Draw PhysicSphere component
    void Inspector::drawPhysicSphere()
    {
        auto physicComp = m_targetObject->getComponent<PhysicSphere>();
        if (physicComp == nullptr)
            return;

        drawPhysicBase();

        m_physicGroup->createWidget<Separator>();
        std::array radius = {physicComp->getRadius()};
        m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.01f, 0.01f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicSphere>();
            physicComp->setRadius(val[0]);
        });
    }

    //! Draw PhysicCapsule component
    void Inspector::drawPhysicCapsule()
    {
        auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
        if (physicComp == nullptr)
            return;

        drawPhysicBase();

        m_physicGroup->createWidget<Separator>();
        std::array height = {physicComp->getHeight()};
        m_physicGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.01f, 0.01f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
            physicComp->setHeight(val[0]);
        });
        std::array radius = {physicComp->getRadius()};
        m_physicGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.01f, 0.01f)->getOnDataChangedEvent().addListener([this](auto &val) {
            auto physicComp = m_targetObject->getComponent<PhysicCapsule>();
            physicComp->setRadius(val[0]);
        });
    }

    //! Draw PhysicCapsule component
    void Inspector::drawAudioSource()
    {
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

        auto txtPath = m_audioSourceGroup->createWidget<TextField>("Path", audioSourceComp->getPath().c_str(), true);
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
        auto audioListenerComp = m_targetObject->getComponent<AudioListener>();
        if (audioListenerComp == nullptr)
            return;

        m_audioListenerGroup->createWidget<CheckBox>("Enable", audioListenerComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
            auto audioListenerComp = m_targetObject->getComponent<AudioListener>();
            audioListenerComp->setEnabled(val);
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

        removeAllWidgets();
    }

    void Inspector::setTargetObject(const std::shared_ptr<SceneObject> &obj)
    {
        if (m_targetObject != obj)
        {
            m_targetObject = obj;
            clear();
            if (m_targetObject != nullptr)
                initialize();
        }
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
