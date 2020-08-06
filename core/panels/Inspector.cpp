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
using namespace ige::scene;

#include <pyxieUtilities.h>
using namespace pyxie;

namespace ige::creator
{
    enum class ComponentType {
        Camera = 0,
        Environment,
        Figure,
        Sprite,
        Script,
        UIImage,
        UIText,
    };

    Inspector::Inspector(const std::string& name, const Panel::Settings& settings)
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

        if (m_targetObject == nullptr) return;

        m_headerGroup = createWidget<Group>("Inspector_Header", false);

        // Object info
        auto columns = m_headerGroup->createWidget<Columns<2>>();
        columns->createWidget<TextField>("ID", std::to_string(m_targetObject->getId()).c_str(), true);
        columns->createWidget<CheckBox>("Active", m_targetObject->isActive())->getOnDataChangedEvent().addListener([this](bool active) {
            m_targetObject->setActive(active);
        });
        m_headerGroup->createWidget<TextField>("Name", m_targetObject->getName().c_str())->getOnDataChangedEvent().addListener([this](auto name) {
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
        }
        else // GUI Object
        {
            m_createCompCombo->addChoice((int)ComponentType::UIImage, "UIImage");
            m_createCompCombo->addChoice((int)ComponentType::UIText, "UIText");
        }

        // Script component
        m_createCompCombo->addChoice((int)ComponentType::Script, "Script Component");

        auto createCompButton = m_headerGroup->createWidget<Button>("Add", ImVec2(64.f, 0.f));
        createCompButton->getOnClickEvent().addListener([this](auto widget){
            switch(m_createCompCombo->getSelectedIndex())
            {
                case (int)ComponentType::Camera: m_targetObject->addComponent<CameraComponent>("camera"); break;
                case (int)ComponentType::Environment: m_targetObject->addComponent<EnvironmentComponent>("environment"); break;
                case (int)ComponentType::Script: m_targetObject->addComponent<ScriptComponent>(); break;
                case (int)ComponentType::Figure: m_targetObject->addComponent<FigureComponent>(); break;
                case (int)ComponentType::Sprite: m_targetObject->addComponent<SpriteComponent>(); break;
                case (int)ComponentType::UIImage: m_targetObject->addComponent<UIImage>(); break;
                case (int)ComponentType::UIText: m_targetObject->addComponent<UIText>("Text", "fonts/Manjari-Regular.ttf"); break;
            }
            redraw();
        });

        // Component
        m_headerGroup->createWidget<Separator>();
        m_componentGroup = createWidget<Group>("Inspector_Components", false);
        std::for_each(m_targetObject->getComponents().begin(), m_targetObject->getComponents().end(), [this](auto& component)
        {
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
            else if (component->getName() == "UIText")
            {
                m_uiTextGroup = header->createWidget<Group>("UITextGroup", false);
                drawUIText();
            }
        });
    }

    void Inspector::drawLocalTransformComponent()
    {
        m_localTransformGroup->removeAllWidgets();
        auto transform = m_targetObject->getTransform();
        if (transform == nullptr) return;

        m_localTransformGroup->createWidget<Label>("Local");

        std::array pos = { transform->getPosition().X(), transform->getPosition().Y(), transform->getPosition().Z() };
        m_localTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            transform->setPosition({ val[0], val[1], val[2] });
            transform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

        Vec3 euler;
        vmath_quatToEuler(transform->getRotation().P(), euler.P());
        std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
        m_localTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            Quat quat;
            float rad[3] = { DEGREES_TO_RADIANS (val[0]), DEGREES_TO_RADIANS (val[1]), DEGREES_TO_RADIANS (val[2])};
            vmath_eulerToQuat(rad, quat.P());
            transform->setRotation(quat);
            transform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

        std::array scale = { transform->getScale().X(), transform->getScale().Y(), transform->getScale().Z() };
        m_localTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            transform->setScale({ val[0], val[1], val[2] });
            transform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

    }

    void Inspector::drawWorldTransformComponent()
    {
        m_worldTransformGroup->removeAllWidgets();
        auto transform = m_targetObject->getTransform();
        if (transform == nullptr) return;

        m_worldTransformGroup->createWidget<Label>("World");
        std::array pos = { transform->getWorldPosition().X(), transform->getWorldPosition().Y(), transform->getWorldPosition().Z() };
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            transform->setWorldPosition({ val[0], val[1], val[2] });
            drawLocalTransformComponent();
        });

        Vec3 euler;
        vmath_quatToEuler(transform->getWorldRotation().P(), euler.P());
        std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            Quat quat;
            float rad[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
            vmath_eulerToQuat(rad, quat.P());
            transform->setWorldRotation(quat);
            drawLocalTransformComponent();
        });

        std::array scale = { transform->getWorldScale().X(), transform->getWorldScale().Y(), transform->getWorldScale().Z() };
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            auto transform = m_targetObject->getTransform();
            transform->setWorldScale({ val[0], val[1], val[2] });
            drawLocalTransformComponent();
        });
    }

    void Inspector::drawCameraComponent()
    {
        if (m_cameraCompGroup == nullptr) return;
        auto camera = m_targetObject->getComponent<CameraComponent>();
        if (camera == nullptr) return;
        m_cameraCompGroup->removeAllWidgets();

        auto columns = m_cameraCompGroup->createWidget<Columns<3>>(120.f);
        // Orthographic
        std::array orthorW = { camera->getOrthoWidth() };
        columns->createWidget<Drag<float>>("OrtW", ImGuiDataType_Float, orthorW)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setOrthoWidth(val[0]);
        });
        std::array orthorH = { camera->getOrthoHeight() };
        columns->createWidget<Drag<float>>("OrtH", ImGuiDataType_Float, orthorH)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setOrthoHeight(val[0]);
        });
        columns->createWidget<CheckBox>("IsOrtho", camera->isOrthoProjection())->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setOrthoProjection(val);
        });

        // FOV - Near - Far
        std::array fov = { camera->getFieldOfView() };
        columns->createWidget<Drag<float>>("FOV", ImGuiDataType_Float, fov)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setFieldOfView(val[0]);
        });
        std::array camNear = { camera->getNearPlane() };
        columns->createWidget<Drag<float>>("Near", ImGuiDataType_Float, camNear)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setNearPlane(val[0]);
        });
        std::array camFar = { camera->getFarPlane() };
        columns->createWidget<Drag<float>>("Far", ImGuiDataType_Float, camFar)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setFarPlane(val[0]);
        });

        // Target
        auto drawCameraLockTarget = [this]() {
            m_cameraLockTargetGroup->removeAllWidgets();

            auto camera = m_targetObject->getComponent<CameraComponent>();
            if (camera->getLockOn()) {
                std::array target = { camera->getTarget().X(), camera->getTarget().Y(), camera->getTarget().Z() };
                auto targetGroup = m_cameraLockTargetGroup->createWidget<Drag<float, 3>>("Target", ImGuiDataType_Float, target);
                targetGroup->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = m_targetObject->getComponent<CameraComponent>();
                    camera->setTarget({ val[0], val[1], val[2] });
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                });
            }
            else {
                // Pan - Tilt - Roll
                auto columns = m_cameraLockTargetGroup->createWidget<Columns<3>>(120.f);
                std::array pan = { RADIANS_TO_DEGREES(camera->getPan()) };
                columns->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan)->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = m_targetObject->getComponent<CameraComponent>();
                    camera->setPan(DEGREES_TO_RADIANS(val[0]));
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                    });
                std::array tilt = { RADIANS_TO_DEGREES(camera->getTilt()) };
                columns->createWidget<Drag<float>>("Tilt", ImGuiDataType_Float, tilt)->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = m_targetObject->getComponent<CameraComponent>();
                    camera->setTilt(DEGREES_TO_RADIANS(val[0]));
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                    });
                std::array roll = { RADIANS_TO_DEGREES(camera->getRoll()) };
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
        std::array ratio = { camera->getAspectRatio() };
        widthBasedColumns->createWidget<Drag<float>>("Ratio", ImGuiDataType_Float, ratio)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = m_targetObject->getComponent<CameraComponent>();
            camera->setAspectRatio(val[0]);
        });
    }

    void Inspector::drawEnvironmentComponent()
    {
        if (m_environmentCompGroup == nullptr) return;
        m_environmentCompGroup->removeAllWidgets();
        auto environment = m_targetObject->getComponent<EnvironmentComponent>();
        if (environment == nullptr) return;

        // Ambient
        auto ambientGroup = m_environmentCompGroup->createWidget<Group>("AmbientLight");
        std::array ambientSkyColor = { environment->getAmbientSkyColor().X(), environment->getAmbientSkyColor().Y(), environment->getAmbientSkyColor().Z()};
        ambientGroup->createWidget<Drag<float, 3>>("SkyColor", ImGuiDataType_Float, ambientSkyColor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setAmbientSkyColor({val[0], val[1], val[2]});
        });
        std::array ambientGroundColor = { environment->getAmbientGroundColor().X(), environment->getAmbientGroundColor().Y(), environment->getAmbientGroundColor().Z() };
        ambientGroup->createWidget<Drag<float, 3>>("GroundColor", ImGuiDataType_Float, ambientGroundColor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setAmbientGroundColor({ val[0], val[1], val[2] });
        });
        std::array ambientDir = { environment->getAmbientDirection().X(), environment->getAmbientDirection().Y(), environment->getAmbientDirection().Z() };
        ambientGroup->createWidget<Drag<float, 3>>("Direction", ImGuiDataType_Float, ambientDir)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setAmbientDirection({ val[0], val[1], val[2] });
        });

        // Directional
        auto directionalGroup = m_environmentCompGroup->createWidget<Group>("DirectionalLight");
        for (int i = 0; i < 3; i++)
        {
            directionalGroup->createWidget<Label>("Light "+ std::to_string(i));
            std::array directCol = { environment->getDirectionalLightColor(i).X(), environment->getDirectionalLightColor(i).Y(), environment->getDirectionalLightColor(i).Z() };
            directionalGroup->createWidget<Drag<float, 3>>("Color", ImGuiDataType_Float, directCol, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setDirectionalLightColor(i, { val[0], val[1], val[2] });
            });
            std::array dir = { environment->getDirectionalLightDirection(i).X(), environment->getDirectionalLightDirection(i).Y(), environment->getDirectionalLightDirection(i).Z() };
            directionalGroup->createWidget<Drag<float, 3>>("Direction", ImGuiDataType_Float, dir)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setDirectionalLightDirection(i, { val[0], val[1], val[2] });
            });
            std::array intensity = { environment->getDirectionalLightIntensity(i) };
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
            std::array color = { environment->getPointLightColor(i).X(), environment->getPointLightColor(i).Y(), environment->getPointLightColor(i).Z() };
            pointGroup->createWidget<Drag<float, 3>>("Color", ImGuiDataType_Float, color, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setPointLightColor(i, { val[0], val[1], val[2] });
            });
            std::array pos = { environment->getPointLightPosition(i).X(), environment->getPointLightPosition(i).Y(), environment->getPointLightPosition(i).Z() };
            pointGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setPointLightPosition(i, { val[0], val[1], val[2] });
            });

            auto col2 = pointGroup->createWidget <Columns<2>>(140.f);
            std::array intensity = { environment->getPointLightIntensity(i) };
            col2->createWidget<Drag<float>>("Int.", ImGuiDataType_Float, intensity)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setPointLightIntensity(i, val[0]);
            });
            std::array range = { environment->getPointLightRange(i) };
            col2->createWidget<Drag<float>>("Range", ImGuiDataType_Float, range)->getOnDataChangedEvent().addListener([i, this](auto val) {
                auto environment = m_targetObject->getComponent<EnvironmentComponent>();
                environment->setPointLightRange(i, val[0]);
            });
        }

        // Shadow
        auto shadowGroup = m_environmentCompGroup->createWidget<Group>("Shadow");
        std::array shadowColor = { environment->getShadowColor().X(), environment->getShadowColor().Y(), environment->getShadowColor().Z() };
        shadowGroup->createWidget<Drag<float, 3>>("Color", ImGuiDataType_Float, shadowColor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowColor({ val[0], val[1], val[2] });
        });
        auto shadowColumn = shadowGroup->createWidget <Columns<2>>(140.f);
        std::array density = { environment->getShadowDensity() };
        shadowColumn->createWidget<Drag<float>>("Density", ImGuiDataType_Float, density, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowDensity(val[0]);
        });
        std::array wideness = { environment->getShadowWideness() };
        shadowColumn->createWidget<Drag<float>>("Wideness", ImGuiDataType_Float, wideness, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setShadowWideness(val[0]);
        });

        // Fog
        auto fogGroup = m_environmentCompGroup->createWidget<Group>("Fog");
        std::array fogColor = { environment->getDistanceFogColor().X(), environment->getDistanceFogColor().Y(), environment->getDistanceFogColor().Z() };
        fogGroup->createWidget<Drag<float, 3>>("Color", ImGuiDataType_Float, fogColor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogColor({ val[0], val[1], val[2] });
        });
        auto fogColumn = fogGroup->createWidget <Columns<3>>(120.f);
        std::array fogNear = { environment->getDistanceFogNear() };
        fogColumn->createWidget<Drag<float>>("Near", ImGuiDataType_Float, fogNear, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogNear(val[0]);
        });
        std::array fogFar = { environment->getDistanceFogFar() };
        fogColumn->createWidget<Drag<float>>("Far", ImGuiDataType_Float, fogNear, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogFar(val[0]);
        });
        std::array fogAlpha = { environment->getDistanceFogAlpha() };
        fogColumn->createWidget<Drag<float>>("Alpha", ImGuiDataType_Float, fogAlpha, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto environment = m_targetObject->getComponent<EnvironmentComponent>();
            environment->setDistanceFogAlpha(val[0]);
        });
    }

    void Inspector::drawFigureComponent()
    {
        if (m_figureCompGroup == nullptr) return;
        m_figureCompGroup->removeAllWidgets();

        auto figureComp = m_targetObject->getComponent<FigureComponent>();
        if (figureComp == nullptr) return;

        auto txtPath = m_figureCompGroup->createWidget<TextField>("Path", figureComp->getPath().c_str(), true);
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto figureComp = m_targetObject->getComponent<FigureComponent>();
            figureComp->setPath(txt);
        });

        for (const auto& type : GetFileExtensionSuported<FigureComponent>())
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto figureComp = m_targetObject->getComponent<FigureComponent>();
                figureComp->setPath(txt);
                redraw();
            });
        }

        m_figureCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Figure (*.pyxf)", "*.pyxf" }).result();
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
                const char* matName = figure->GetMaterialName(i);
                if (matName)
                {
                    m_figureCompGroup->createWidget<Label>(matName);
                    int index = figure->GetMaterialIndex(GenerateNameHash(matName));
                    auto currMat = figure->GetMaterial(i);

                    for (auto j = 0; j < currMat->numParams; j++)
                    {
                        auto info = RenderContext::Instance().GetShaderParameterInfoByHash(currMat->params[j].hash);
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

                            for (const auto& type : GetFileExtensionSuported<SpriteComponent>())
                            {
                                txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this, index, infoName](auto txt) {
                                    updateMaterial(index, infoName, txt);
                                    redraw();
                                });
                            }
                        }
                        else if ((currMat->params[j].type == ParamTypeFloat))
                        {
                            std::array val = { currMat->params[j].fValue[0] };
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
        if (m_spriteCompGroup == nullptr) return;
        m_spriteCompGroup->removeAllWidgets();

        auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
        if (spriteComp == nullptr) return;

        auto txtPath = m_spriteCompGroup->createWidget<TextField>("Path", spriteComp->getPath().c_str(), true);
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setPath(txt);
        });

        for (const auto& type : GetFileExtensionSuported<SpriteComponent>())
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
                spriteComp->setPath(txt);
                redraw();
            });
        }

        m_spriteCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
            if (files.size() > 0)
            {
                auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
                spriteComp->setPath(files[0]);
                redraw();
            }
        });

        std::array size = { spriteComp->getSize().X(),  spriteComp->getSize().Y() };
        m_spriteCompGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, size, 1.f, 0.f, 4096.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setSize({ val[0], val[1] });
        });
    }

    void Inspector::drawScriptComponent()
    {
        if (m_scriptCompGroup == nullptr) return;
        m_scriptCompGroup->removeAllWidgets();

        auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
        if (scriptComp == nullptr) return;

        auto txtPath = m_scriptCompGroup->createWidget<TextField>("Path", scriptComp->getPath().c_str(), true);
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
            scriptComp->setPath(txt);
        });

        for (const auto& type : GetFileExtensionSuported<ScriptComponent>())
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                scriptComp->setPath(txt);
                redraw();
            });
        }

        m_scriptCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Script (*.py)", "*.py" }).result();
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
        if (m_rectTransformGroup == nullptr) return;
        m_rectTransformGroup->removeAllWidgets();

        auto rectTransform = m_targetObject->getComponent<RectTransform>();
        if (rectTransform == nullptr) return;

        m_rectTransformGroup->createWidget<AnchorPresets>("AnchorPresets")->getOnClickEvent().addListener([this](const auto& widget) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            auto anchor = rectTransform->getAnchor();
            auto anchorWidget = (AnchorWidget*)widget;
            anchor.m_left = anchorWidget->getAnchorMin().x;
            anchor.m_top = anchorWidget->getAnchorMin().y;
            anchor.m_right = anchorWidget->getAnchorMax().x;
            anchor.m_bottom = anchorWidget->getAnchorMax().y;
            rectTransform->setAnchor(anchor);
            redraw();
        });

        auto anchorColumn = m_rectTransformGroup->createWidget<Columns<2>>();
        anchorColumn->setColumnWidth(0, 52.f);
        auto anchor = rectTransform->getAnchor();
        auto offset = rectTransform->getOffset();
        anchorColumn->createWidget<AnchorWidget>(ImVec2(anchor.m_left, anchor.m_top), ImVec2(anchor.m_right, anchor.m_bottom), false)->getOnClickEvent().addListener([](auto widget) {
            ImGui::OpenPopup("AnchorPresets");
        });

        auto anchorGroup = anchorColumn->createWidget<Group>("AnchorGroup", false, false);
        auto anchorGroupColums = anchorGroup->createWidget<Columns<3>>(-1.f, true, 52.f);

        if ((anchor.m_left == 0.f && anchor.m_right == 1.f)
            && (anchor.m_top == 0.f && anchor.m_bottom == 1.f))
        {
            std::array left = { offset.m_left };
            anchorGroupColums->createWidget<Drag<float>>("L", ImGuiDataType_Float, left, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset.m_left = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array top = { offset.m_top };
            anchorGroupColums->createWidget<Drag<float>>("T", ImGuiDataType_Float, top, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset.m_top = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = { rectTransform->getPosition().Z() };
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array right = { offset.m_right };
            anchorGroupColums->createWidget<Drag<float>>("R", ImGuiDataType_Float, right, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset.m_right = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array bottom = { offset.m_bottom };
            anchorGroupColums->createWidget<Drag<float>>("B", ImGuiDataType_Float, bottom, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset.m_bottom = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });
        }
        else if (anchor.m_left == 0.f && anchor.m_right == 1.f)
        {
            std::array left = { offset.m_left };
            anchorGroupColums->createWidget<Drag<float>>("L", ImGuiDataType_Float, left, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset.m_left = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });
            std::array posY = { rectTransform->getPosition().Y() };
            anchorGroupColums->createWidget<Drag<float>>("Y", ImGuiDataType_Float, posY, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Y(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });
            std::array posZ = { rectTransform->getPosition().Z() };
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array right = { offset.m_right };
            anchorGroupColums->createWidget<Drag<float>>("R", ImGuiDataType_Float, right, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset.m_right = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array height = { rectTransform->getSize().Y() };
            anchorGroupColums->createWidget<Drag<float>>("H", ImGuiDataType_Float, height, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.Y(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });
        }
        else if (anchor.m_top == 0.f && anchor.m_bottom == 1.f)
        {
            std::array posX = { rectTransform->getPosition().X() };
            anchorGroupColums->createWidget<Drag<float>>("X", ImGuiDataType_Float, posX, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.X(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array top = { offset.m_top };
            anchorGroupColums->createWidget<Drag<float>>("T", ImGuiDataType_Float, top, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset.m_top = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = { rectTransform->getPosition().Z() };
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array width = { rectTransform->getSize().X() };
            anchorGroupColums->createWidget<Drag<float>>("W", ImGuiDataType_Float, width, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.X(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });

            std::array bottom = { offset.m_bottom };
            anchorGroupColums->createWidget<Drag<float>>("B", ImGuiDataType_Float, bottom, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto offset = rectTransform->getOffset();
                offset.m_bottom = val[0];
                rectTransform->setOffset(offset);
                rectTransform->onUpdate(0.f);
            });
        }
        else
        {
            std::array posX = { rectTransform->getPosition().X() };
            anchorGroupColums->createWidget<Drag<float>>("X", ImGuiDataType_Float, posX, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.X(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array posY = { rectTransform->getPosition().Y() };
            anchorGroupColums->createWidget<Drag<float>>("Y", ImGuiDataType_Float, posY, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Y(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array posZ = { rectTransform->getPosition().Z() };
            anchorGroupColums->createWidget<Drag<float>>("Z", ImGuiDataType_Float, posZ, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto position = rectTransform->getPosition();
                position.Z(val[0]);
                rectTransform->setPosition(position);
                rectTransform->onUpdate(0.f);
            });

            std::array width = { rectTransform->getSize().X() };
            anchorGroupColums->createWidget<Drag<float>>("W", ImGuiDataType_Float, width, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.X(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });

            std::array height = { rectTransform->getSize().Y() };
            anchorGroupColums->createWidget<Drag<float>>("H", ImGuiDataType_Float, height, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
                auto rectTransform = m_targetObject->getComponent<RectTransform>();
                auto size = rectTransform->getSize();
                size.Y(val[0]);
                rectTransform->setSize(size);
                rectTransform->onUpdate(0.f);
            });
        }

        std::array pivot = { rectTransform->getPivot().X(), rectTransform->getPivot().Y() };
        m_rectTransformGroup->createWidget<Drag<float, 2>>("Pivot", ImGuiDataType_Float, pivot, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            rectTransform->setPivot({ val[0], val[1] });
            rectTransform->onUpdate(0.f);
        });

        Vec3 euler;
        vmath_quatToEuler(rectTransform->getRotation().P(), euler.P());
        std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
        m_rectTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            Quat quat;
            float rad[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
            vmath_eulerToQuat(rad, quat.P());
            rectTransform->setRotation(quat);
            rectTransform->onUpdate(0.f);
        });

        std::array scale = { rectTransform->getScale().X(), rectTransform->getScale().Y(), rectTransform->getScale().Z() };
        m_rectTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            rectTransform->setScale({ val[0], val[1], val[2] });
            rectTransform->onUpdate(0.f);
        });
    }

    void Inspector::drawCanvas()
    {
        if (m_canvasGroup == nullptr) return;
        m_canvasGroup->removeAllWidgets();

        auto canvas = m_targetObject->getComponent<ige::scene::Canvas>();
        if (canvas == nullptr) return;

        std::array size = { canvas->getDesignCanvasSize().X(), canvas->getDesignCanvasSize().Y() };
        m_canvasGroup->createWidget<Drag<float, 2>>("Design Size", ImGuiDataType_Float, size)->getOnDataChangedEvent().addListener([this](auto val) {
            auto canvas = m_targetObject->getComponent<ige::scene::Canvas>();
            canvas->setDesignCanvasSize({ val[0], val[1] });
        });

        std::array targetSize = { canvas->getTargetCanvasSize().X(), canvas->getTargetCanvasSize().Y() };
        m_canvasGroup->createWidget<Drag<float, 2>>("Target Size", ImGuiDataType_Float, targetSize)->getOnDataChangedEvent().addListener([this](auto val) {
            auto canvas = m_targetObject->getComponent<ige::scene::Canvas>();
            canvas->setTargetCanvasSize({ val[0], val[1] });
        });
    }

    void Inspector::drawUIImage()
    {
        if (m_uiImageGroup == nullptr) return;
        m_uiImageGroup->removeAllWidgets();

        auto uiImage = m_targetObject->getComponent<UIImage>();
        if (uiImage == nullptr) return;

        auto txtPath = m_uiImageGroup->createWidget<TextField>("Path", uiImage->getPath().c_str(), true);
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            auto uiImage = m_targetObject->getComponent<UIImage>();
            uiImage->setPath(txt);
        });

        for (const auto& type : GetFileExtensionSuported<SpriteComponent>())
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                auto uiImage = m_targetObject->getComponent<UIImage>();
                uiImage->setPath(txt);
                redraw();
            });
        }

        m_uiImageGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
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
        if (m_uiTextGroup == nullptr) return;
        m_uiTextGroup->removeAllWidgets();

        auto uiText = m_targetObject->getComponent<UIText>();
        if (uiText == nullptr) return;

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

        std::array size = { (float)uiText->getFontSize() };
        m_uiTextGroup->createWidget<Drag<float>>("Size", ImGuiDataType_Float, size, 1.f, 4.f, 50.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setFontSize((int)val[0]);
        });

        static Vec4 color4;
        color4 = uiText->getColor();
        auto color = m_uiTextGroup->createWidget<Color>("Color", color4.P());
        color->getOnDataChangedEvent().addListener([this](auto& color) {
            auto uiText = m_targetObject->getComponent<UIText>();
            uiText->setColor({ color[0], color[1], color[2], color[3] });
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
        m_headerGroup = nullptr;
        m_createCompCombo = nullptr;
        m_componentGroup = nullptr;
        m_localTransformGroup = nullptr;
        m_worldTransformGroup = nullptr;
        m_cameraCompGroup = nullptr;
        m_cameraLockTargetGroup = nullptr;
        m_environmentCompGroup = nullptr;
        m_figureCompGroup = nullptr;
        m_spriteCompGroup = nullptr;
        m_rectTransformGroup = nullptr;
        m_canvasGroup = nullptr;
        m_uiImageGroup = nullptr;
        m_uiTextGroup = nullptr;

        removeAllWidgets();
    }

    void Inspector::setTargetObject(std::shared_ptr<SceneObject> obj)
    {
        if (m_targetObject != obj)
        {
            m_targetObject = obj;
            initialize();
        }
    }

    void Inspector::updateMaterial(int index, const char* infoName, std::string txt)
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
}
