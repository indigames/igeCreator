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
#include "core/panels/Inspector.h"
#include "core/Editor.h"

#include <components/CameraComponent.h>
#include <components/TransformComponent.h>
#include <components/EnvironmentComponent.h>
#include <components/FigureComponent.h>
#include <components/EditableFigureComponent.h>
using namespace ige::scene;

#include <pyxieUtilities.h>
using namespace pyxie;

namespace ige::creator
{
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
        auto colums = m_headerGroup->createWidget<Columns<3>>(120);
        colums->createWidget<TextField>("ID", std::to_string(m_targetObject->getId()), true);
        colums->createWidget<TextField>("Name", m_targetObject->getName())->getOnDataChangedEvent().addListener([this](auto name) {
            m_targetObject->setName(name);
        });
        colums->createWidget<CheckBox>("Active", m_targetObject->isActive())->getOnDataChangedEvent().addListener([this](bool active) {
            m_targetObject->setActive(active);
        });

        // Create component selection
        m_createCompCombo = m_headerGroup->createWidget<ComboBox>();
        m_createCompCombo->setEndOfLine(false);
        m_createCompCombo->addChoice(0, "Camera");
        m_createCompCombo->addChoice(1, "EditableFigure");
        m_createCompCombo->addChoice(2, "Environment");
        m_createCompCombo->addChoice(3, "Figure");
        m_createCompCombo->addChoice(4, "Transform");

        auto createCompButton = m_headerGroup->createWidget<Button>("Add Component", ImVec2(100.f, 0.f));
        createCompButton->getOnClickEvent().addListener([this](){
            switch(m_createCompCombo->getSelectedIndex())
            {
                case 0: getTargetObject()->addComponent<CameraComponent>("camera"); break;
                case 1: getTargetObject()->addComponent<EditableFigureComponent>("editableFigure"); break;
                case 2: getTargetObject()->addComponent<EnvironmentComponent>("environment"); break;
                case 3: getTargetObject()->addComponent<FigureComponent>("figure"); break;
                case 4: getTargetObject()->addComponent<TransformComponent>(); break;
            }
        });

        // Component
        m_headerGroup->createWidget<Separator>();
        m_componentGroup = createWidget<Group>("Inspector_Components", false);
        std::for_each(getTargetObject()->getComponents().begin(), getTargetObject()->getComponents().end(), [this](auto& component)
        {
            auto closable = (component->getName() != "TransformComponent");
            auto header = m_componentGroup->createWidget<Group>(component->getName(), true, closable);
            header->getOnClosedEvent().addListener([this, &component]() {
                getTargetObject()->removeComponent(component);
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
            else if (component->getName() == "EditableFigureComponent")
            {
                m_editableFigureCompGroup = header->createWidget<Group>("EditableFigureGroup", false);
                drawEditableFigureComponent();
            }
        });
    }

    void Inspector::drawLocalTransformComponent()
    {
        m_localTransformGroup->removeAllWidgets();
        auto tranform = getTargetObject()->getComponent<TransformComponent>();
        if (tranform == nullptr) return;

        m_localTransformGroup->createWidget<Label>("Local");

        std::array pos = { tranform->getPosition().X(), tranform->getPosition().Y(), tranform->getPosition().Z() };
        m_localTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
            auto tranform = getTargetObject()->getComponent<TransformComponent>();
            tranform->setPosition({ val[0], val[1], val[2] });
            tranform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

        Vec3 euler;
        vmath_quatToEuler(tranform->getRotation().P(), euler.P());
        std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
        m_localTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            auto tranform = getTargetObject()->getComponent<TransformComponent>();
            Quat quat;
            float rad[3] = { DEGREES_TO_RADIANS (val[0]), DEGREES_TO_RADIANS (val[1]), DEGREES_TO_RADIANS (val[2])};
            vmath_eulerToQuat(rad, quat.P());
            tranform->setRotation(quat);
            tranform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

        std::array scale = { tranform->getScale().X(), tranform->getScale().Y(), tranform->getScale().Z() };
        m_localTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            auto tranform = getTargetObject()->getComponent<TransformComponent>();
            tranform->setScale({ val[0], val[1], val[2] });
            tranform->onUpdate(0.f);
            drawWorldTransformComponent();
        });

    }

    void Inspector::drawWorldTransformComponent()
    {
        m_worldTransformGroup->removeAllWidgets();
        auto tranform = getTargetObject()->getComponent<TransformComponent>();
        if (tranform == nullptr) return;

        m_worldTransformGroup->createWidget<Label>("World");
        std::array pos = { tranform->getWorldPosition().X(), tranform->getWorldPosition().Y(), tranform->getWorldPosition().Z() };
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos)->getOnDataChangedEvent().addListener([this](auto val) {
            auto tranform = getTargetObject()->getComponent<TransformComponent>();
            tranform->setWorldPosition({ val[0], val[1], val[2] });
            drawLocalTransformComponent();
        });

        Vec3 euler;
        vmath_quatToEuler(tranform->getWorldRotation().P(), euler.P());
        std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot)->getOnDataChangedEvent().addListener([this](auto val) {
            auto tranform = getTargetObject()->getComponent<TransformComponent>();
            Quat quat;
            float rad[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
            vmath_eulerToQuat(rad, quat.P());
            tranform->setWorldRotation(quat);
            drawLocalTransformComponent();
        });

        std::array scale = { tranform->getWorldScale().X(), tranform->getWorldScale().Y(), tranform->getWorldScale().Z() };
        m_worldTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale)->getOnDataChangedEvent().addListener([this](auto val) {
            auto tranform = getTargetObject()->getComponent<TransformComponent>();
            tranform->setWorldScale({ val[0], val[1], val[2] });
            drawLocalTransformComponent();
        });
    }

    void Inspector::drawCameraComponent()
    {
        if (m_cameraCompGroup == nullptr) return;
        auto camera = getTargetObject()->getComponent<CameraComponent>();
        if (camera == nullptr) return;
        m_cameraCompGroup->removeAllWidgets();

        auto colums = m_cameraCompGroup->createWidget<Columns<3>>(120);
        // Orthographic
        std::array orthorW = { camera->getOrthoWidth() };
        colums->createWidget<Drag<float>>("OrtW", ImGuiDataType_Float, orthorW)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            camera->setOrthoWidth(val[0]);
        });
        std::array orthorH = { camera->getOrthoHeight() };
        colums->createWidget<Drag<float>>("OrtH", ImGuiDataType_Float, orthorH)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            camera->setOrthoHeight(val[0]);
        });
        colums->createWidget<CheckBox>("IsOrtho", camera->isOrthoProjection())->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            camera->setOrthoProjection(val);
        });

        // FOV - Near - Far
        std::array fov = { camera->getFieldOfView() };
        colums->createWidget<Drag<float>>("FOV", ImGuiDataType_Float, fov)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            camera->setFieldOfView(val[0]);
        });
        std::array camNear = { camera->getNearPlane() };
        colums->createWidget<Drag<float>>("Near", ImGuiDataType_Float, camNear)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            camera->setNearPlane(val[0]);
        });
        std::array camFar = { camera->getFarPlane() };
        colums->createWidget<Drag<float>>("Far", ImGuiDataType_Float, camFar)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            camera->setFarPlane(val[0]);
        });
        
        // Target
        auto drawCameraLockTarget = [this]() {
            m_cameraLockTargetGroup->removeAllWidgets();

            auto camera = getTargetObject()->getComponent<CameraComponent>();
            if (camera->getLockon()) {
                std::array target = { camera->getTarget().X(), camera->getTarget().Y(), camera->getTarget().Z() };
                auto targetGroup = m_cameraLockTargetGroup->createWidget<Drag<float, 3>>("Target", ImGuiDataType_Float, target);
                targetGroup->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = getTargetObject()->getComponent<CameraComponent>();
                    camera->setTarget({ val[0], val[1], val[2] });
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                });
            }
            else {
                // Pan - Tilt - Roll
                auto colums = m_cameraLockTargetGroup->createWidget<Columns<3>>(120);
                std::array pan = { RADIANS_TO_DEGREES(camera->getPan()) };
                colums->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan)->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = getTargetObject()->getComponent<CameraComponent>();
                    camera->setPan(DEGREES_TO_RADIANS(val[0]));
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                    });
                std::array tilt = { RADIANS_TO_DEGREES(camera->getTilt()) };
                colums->createWidget<Drag<float>>("Tilt", ImGuiDataType_Float, tilt)->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = getTargetObject()->getComponent<CameraComponent>();
                    camera->setTilt(DEGREES_TO_RADIANS(val[0]));
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                    });
                std::array roll = { RADIANS_TO_DEGREES(camera->getRoll()) };
                colums->createWidget<Drag<float>>("Roll", ImGuiDataType_Float, roll)->getOnDataChangedEvent().addListener([this](auto val) {
                    auto camera = getTargetObject()->getComponent<CameraComponent>();
                    camera->setRoll(DEGREES_TO_RADIANS(val[0]));
                    drawLocalTransformComponent();
                    drawWorldTransformComponent();
                });
            }
        };
               
        m_cameraCompGroup->createWidget<CheckBox>("LockTarget", camera->getLockon())->getOnDataChangedEvent().addListener([drawCameraLockTarget, this](auto locked) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            if (!locked)
            {
                auto tranform = getTargetObject()->getComponent<TransformComponent>();
                camera->setTarget(tranform->getPosition() + Vec3(0.f, 0.f, -1.f));
            }
            else
            {
                camera->setPan(0.f);
                camera->setTilt(0.f);
                camera->setRoll(0.f);
            }
            camera->lockonTarget(locked);
            drawLocalTransformComponent();
            drawWorldTransformComponent();
            drawCameraLockTarget();
          });

        m_cameraLockTargetGroup = m_cameraCompGroup->createWidget<Group>("LockTargetGroup", false);
        drawCameraLockTarget();

        // Width Based
        auto widthBasedColums = m_cameraCompGroup->createWidget<Columns<2>>(180);
        widthBasedColums->createWidget<CheckBox>("WidthBased", camera->isWidthBase())->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            camera->setWidthBase(val);
        });

        // Aspect Ratio
        std::array ratio = { camera->getAspectRatio() };
        widthBasedColums->createWidget<Drag<float>>("Ratio", ImGuiDataType_Float, ratio)->getOnDataChangedEvent().addListener([this](auto val) {
            auto camera = getTargetObject()->getComponent<CameraComponent>();
            camera->setAspectRatio(val[0]);
        });
    }

    void Inspector::drawEnvironmentComponent()
    {
        if (m_environmentCompGroup == nullptr) return;
        m_environmentCompGroup->removeAllWidgets();
    }

    void Inspector::drawFigureComponent()
    {
        if (m_figureCompGroup == nullptr) return;
        m_figureCompGroup->removeAllWidgets();
    }

    void Inspector::drawEditableFigureComponent()
    {
        if (m_editableFigureCompGroup == nullptr) return;
        m_editableFigureCompGroup->removeAllWidgets();
    }
    
    void Inspector::_drawImpl()
    {
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
        m_editableFigureCompGroup = nullptr;

        removeAllWidgets();
    }

    void Inspector::setTargetObject(std::shared_ptr<SceneObject> obj)
    { 
        m_targetObject = obj;

        initialize();
    }
}
