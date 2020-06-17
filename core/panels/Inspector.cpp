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

namespace ige::creator
{
    Inspector::Inspector(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
    }
    
    Inspector::~Inspector()
    {
    }

    void Inspector::initialize()
    {
        clear();

        if (m_targetObject == nullptr) return;

        m_headerGroup = createWidget<Group>("Inspector_Header", false);

        // Object info
        m_headerGroup->createWidget<TextField>("ID", std::to_string(m_targetObject->getId()), true);
        m_headerGroup->createWidget<TextField>("Name", m_targetObject->getName())->getOnDataChangedEvent().addListener([this](auto txt) {
            m_targetObject->setName(txt);
        });
        m_headerGroup->createWidget<CheckBox>("Active", m_targetObject->isActive())->getOnDataChangedEvent().addListener([this](bool active) {
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
                case 0: getTargetObject()->addComponent<CameraComponent>("Camera"); break;
                case 1: getTargetObject()->addComponent<EditableFigureComponent>("EFifure"); break;
                case 2: getTargetObject()->addComponent<EnvironmentComponent>("Envieromnent"); break;
                case 3: getTargetObject()->addComponent<FigureComponent>("Figure"); break;
                case 4: getTargetObject()->addComponent<TransformComponent>(); break;                
            }
        });

        // Component
        m_headerGroup->createWidget<Separator>();
        m_componentGroup = createWidget<Group>("Inspector_Components", false);
        std::for_each(getTargetObject()->getComponents().begin(), getTargetObject()->getComponents().end(), [this](auto& component)
            {
                auto closable = (component->getName() == "TransformComponent");
                auto header = m_componentGroup->createWidget<Group>(component->getName(), true, closable);
                header->getOnClosedEvent().addListener([this, component]() {
                    getTargetObject()->removeComponent(component);
                    });
                
                if (component->getName() == "TransformComponent")
                {
                    auto localGroup = header->createWidget<Group>("Local", false);
                    localGroup->createWidget<Label>("Local");
                    auto tranform = std::static_pointer_cast<TransformComponent>(component);
                    std::array pos = { tranform->getPosition().X(), tranform->getPosition().Y(), tranform->getPosition().Z() };
                    auto posDrag = localGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos);
                    posDrag->getOnDataChangedEvent().addListener([&component](auto val) {
                        auto tranform = (TransformComponent*)(component.get());
                        tranform->setPosition({ val[0], val[1], val[2] });
                    });

                    Vec3 euler;
                    vmath_quatToEuler(tranform->getRotation().P(), euler.P());
                    std::array rot = { euler.X(), euler.Y(), euler.Z()};
                    auto rotDrag = localGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot);
                    rotDrag->getOnDataChangedEvent().addListener([&component](auto val) {
                        auto tranform = (TransformComponent*)(component.get());
                        Quat quat;
                        vmath_eulerToQuat(val.data(), quat.P());
                        tranform->setRotation(quat);
                    });

                    std::array scale = { tranform->getScale().X(), tranform->getScale().Y(), tranform->getScale().Z() };
                    auto scaleDrag = localGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale);
                    scaleDrag->getOnDataChangedEvent().addListener([&component](auto val) {
                        auto tranform = (TransformComponent*)(component.get());
                        tranform->setScale({ val[0], val[1], val[2] });
                    });

                    auto worldGroup = header->createWidget<Group>("World", false);
                    localGroup->createWidget<Label>("World");
                    pos = { tranform->getWorldPosition().X(), tranform->getWorldPosition().Y(), tranform->getWorldPosition().Z() };
                    worldGroup->createWidget<Drag<float, 3>>("Position", ImGuiDataType_Float, pos, 0.f);
                    
                    vmath_quatToEuler(tranform->getWorldRotation().P(), euler.P());
                    rot = { euler.X(), euler.Y(), euler.Z() };
                    worldGroup->createWidget<Drag<float, 3>>("Rotation", ImGuiDataType_Float, rot, 0.f);
                    
                    scale = { tranform->getWorldScale().X(), tranform->getWorldScale().Y(), tranform->getWorldScale().Z() };
                    worldGroup->createWidget<Drag<float, 3>>("Scale", ImGuiDataType_Float, scale, 0.f);
                }
                else if (component->getName() == "CameraComponent")
                {

                }
                else if (component->getName() == "EnvironmentComponent")
                {

                }
                else if (component->getName() == "FigureComponent")
                {

                }
                else if (component->getName() == "EditableFigureComponent")
                {

                }
            });
    }

    void Inspector::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void Inspector::clear()
    {
        removeAllWidgets();
    }

    void Inspector::setTargetObject(std::shared_ptr<SceneObject> obj)
    { 
        m_targetObject = obj;

        initialize();
    }
}
