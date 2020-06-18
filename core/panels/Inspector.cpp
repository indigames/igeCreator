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
                drawLocalTransformComponent(component);

                m_worldTransformGroup = header->createWidget<Group>("WorldTransformGroup", false);
                drawWorldTransformComponent(component);
            }
            else if (component->getName() == "CameraComponent")
            {
                m_cameraCompGroup = header->createWidget<Group>("CameraGroup", false);
                drawCameraComponent(component);
            }
            else if (component->getName() == "EnvironmentComponent")
            {
                m_environmentCompGroup = header->createWidget<Group>("EnvironmentGroup", false);
                drawEnvironmentComponent(component);
            }
            else if (component->getName() == "FigureComponent")
            {
                m_figureCompGroup = header->createWidget<Group>("FigureGroup", false);
                drawFigureComponent(component);
            }
            else if (component->getName() == "EditableFigureComponent")
            {
                m_editableFigureCompGroup = header->createWidget<Group>("EditableFigureGroup", false);
                drawEditableFigureComponent(component);
            }
        });
    }

    void Inspector::drawLocalTransformComponent(std::shared_ptr<Component>& component)
    {
        m_localTransformGroup->removeAllWidgets();
        m_localTransformGroup->createWidget<Label>("Local");

        auto tranform = std::static_pointer_cast<TransformComponent>(component);
        std::array pos = { tranform->getPosition().X(), tranform->getPosition().Y(), tranform->getPosition().Z() };
        auto posDrag = m_localTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos);
        posDrag->getOnDataChangedEvent().addListener([&component, this](auto val) {
            auto tranform = (TransformComponent*)(component.get());
            tranform->setPosition({ val[0], val[1], val[2] });
            tranform->onUpdate(0.f);
            drawWorldTransformComponent(component);
        });

        Vec3 euler;
        vmath_quatToEuler(tranform->getRotation().P(), euler.P());
        std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
        auto rotDrag = m_localTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot);
        rotDrag->getOnDataChangedEvent().addListener([&component, this](auto val) {
            auto tranform = (TransformComponent*)(component.get());
            Quat quat;
            float rad[3] = { DEGREES_TO_RADIANS (val[0]), DEGREES_TO_RADIANS (val[1]), DEGREES_TO_RADIANS (val[2])};
            vmath_eulerToQuat(rad, quat.P());
            tranform->setRotation(quat);
            tranform->onUpdate(0.f);
            drawWorldTransformComponent(component);
        });

        std::array scale = { tranform->getScale().X(), tranform->getScale().Y(), tranform->getScale().Z() };
        auto scaleDrag = m_localTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale);
        scaleDrag->getOnDataChangedEvent().addListener([&component, this](auto val) {
            auto tranform = (TransformComponent*)(component.get());
            tranform->setScale({ val[0], val[1], val[2] });
            tranform->onUpdate(0.f);
            drawWorldTransformComponent(component);
        });

    }

    void Inspector::drawWorldTransformComponent(std::shared_ptr<Component>& component)
    {
        m_worldTransformGroup->removeAllWidgets();
        m_worldTransformGroup->createWidget<Label>("World");

        auto tranform = std::static_pointer_cast<TransformComponent>(component);
        std::array pos = { tranform->getWorldPosition().X(), tranform->getWorldPosition().Y(), tranform->getWorldPosition().Z() };
        auto worldPosDrag = m_worldTransformGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos);
        worldPosDrag->getOnDataChangedEvent().addListener([&component, this](auto val) {
            auto tranform = (TransformComponent*)(component.get());
            tranform->setWorldPosition({ val[0], val[1], val[2] });
            drawLocalTransformComponent(component);
        });

        Vec3 euler;
        vmath_quatToEuler(tranform->getWorldRotation().P(), euler.P());
        std::array rot = { RADIANS_TO_DEGREES(euler.X()), RADIANS_TO_DEGREES(euler.Y()), RADIANS_TO_DEGREES(euler.Z()) };
        auto worldRotDrag = m_worldTransformGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot);
        worldRotDrag->getOnDataChangedEvent().addListener([&component, this](auto val) {
            auto tranform = (TransformComponent*)(component.get());
            Quat quat;
            float rad[3] = { DEGREES_TO_RADIANS(val[0]), DEGREES_TO_RADIANS(val[1]), DEGREES_TO_RADIANS(val[2]) };
            vmath_eulerToQuat(rad, quat.P());
            tranform->setWorldRotation(quat);
            drawLocalTransformComponent(component);
        });

        std::array scale = { tranform->getWorldScale().X(), tranform->getWorldScale().Y(), tranform->getWorldScale().Z() };
        auto worldScaleDrag = m_worldTransformGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale);
        worldScaleDrag->getOnDataChangedEvent().addListener([&component, this](auto val) {
            auto tranform = (TransformComponent*)(component.get());
            tranform->setWorldScale({ val[0], val[1], val[2] });
            drawLocalTransformComponent(component);
        });
    }

    void Inspector::drawCameraComponent(std::shared_ptr<Component>& component)
    {
        if (m_cameraCompGroup == nullptr) return;
        m_cameraCompGroup->removeAllWidgets();

        auto camera = std::static_pointer_cast<CameraComponent>(component);
        auto colums = m_cameraCompGroup->createWidget<Columns<3>>(120);

        // Orthographic
        colums->createWidget<CheckBox>("IsOrtho", camera->isOrthoProjection())->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setOrthoProjection(val);
        });
        std::array orthorW = { camera->getOrthoWidth() };
        colums->createWidget<Drag<float>>("OrtW", ImGuiDataType_Float, orthorW)->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setOrthoWidth(val[0]);
        });
        std::array orthorH = { camera->getOrthoWidth() };
        colums->createWidget<Drag<float>>("OrtH", ImGuiDataType_Float, orthorH)->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setOrthoWidth(val[0]);
        });

        // FOV - Near - Far
        std::array fov = { camera->getFieldOfView() };
        colums->createWidget<Drag<float>>("FOV", ImGuiDataType_Float, fov)->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setFieldOfView(val[0]);
        });
        std::array camNear = { camera->getNearPlane() };
        colums->createWidget<Drag<float>>("Near", ImGuiDataType_Float, camNear)->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setNearPlane(val[0]);
        });
        std::array camFar = { camera->getFarPlane() };
        colums->createWidget<Drag<float>>("Far", ImGuiDataType_Float, camFar)->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setFarPlane(val[0]);
        });

        // Pan - Titl - Roll
        std::array pan = { camera->getPan() };
        colums->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan)->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setPan(val[0]);
        });
        std::array tilt = { camera->getTilt() };
        colums->createWidget<Drag<float>>("Tilt", ImGuiDataType_Float, tilt)->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setTilt(val[0]);
        });
        std::array roll = { camera->getRoll() };
        colums->createWidget<Drag<float>>("Roll", ImGuiDataType_Float, roll)->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setRoll(val[0]);
        });

        // Width based?
        colums->createWidget<CheckBox>("WidthBased", camera->isWidthBase())->getOnDataChangedEvent().addListener([&component](auto val) {
            auto camera = std::static_pointer_cast<CameraComponent>(component);
            camera->setWidthBase(val);
        });
    }

    void Inspector::drawEnvironmentComponent(std::shared_ptr<Component>& component)
    {
        if (m_environmentCompGroup == nullptr) return;
        m_environmentCompGroup->removeAllWidgets();
    }

    void Inspector::drawFigureComponent(std::shared_ptr<Component>& component)
    {
        if (m_figureCompGroup == nullptr) return;
        m_figureCompGroup->removeAllWidgets();
    }

    void Inspector::drawEditableFigureComponent(std::shared_ptr<Component>& component)
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
