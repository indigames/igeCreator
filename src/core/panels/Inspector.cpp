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
#include "core/menu/Menu.h"
#include "core/menu/MenuItem.h"
#include "core/panels/Inspector.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/FileHandle.h"
#include "core/task/TaskManager.h"

#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"

#include <components/CompoundComponent.h>
#include <components/CameraComponent.h>
#include <components/TransformComponent.h>
#include <components/EnvironmentComponent.h>
#include <components/FigureComponent.h>
#include <components/BoneTransform.h>
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
#include <components/gui/UIButton.h>
#include <components/gui/UISlider.h>
#include <components/gui/UIScrollView.h>
#include <components/gui/UIScrollBar.h>
#include <components/gui/UIMask.h>
#include <components/physic/PhysicManager.h>
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
#include <components/audio/AudioManager.h>
#include <components/audio/AudioSource.h>
#include <components/audio/AudioListener.h>
#include <components/particle/ParticleManager.h>
#include <components/particle/Particle.h>
#include <components/navigation/NavMesh.h>
#include <components/navigation/NavAgent.h>
#include <components/navigation/NavAgentManager.h>
#include <components/navigation/Navigable.h>
#include <components/navigation/DynamicNavMesh.h>
#include <components/navigation/NavObstacle.h>
#include <components/navigation/OffMeshLink.h>

#include "core/panels/InspectorEditor.h"

#include <scene/Scene.h>
#include <scene/TargetObject.h>
using namespace ige::scene;

#include <pyxieUtilities.h>
using namespace pyxie;

namespace ige::creator
{
    IgnoreTransformEventScope::IgnoreTransformEventScope(std::shared_ptr<Component> comp, uint64_t& eventId, const std::function<void(SceneObject&)>& task)
        : m_comp(comp), m_eventId(eventId), m_task(task)
    {
        if (m_comp)
        {
            if (m_eventId != (uint64_t)-1)
                m_comp->getOwner()->getTransformChangedEvent().removeListener(m_eventId);
        }
    }

    IgnoreTransformEventScope::~IgnoreTransformEventScope()
    {
        if (m_comp)
        {
            m_eventId = m_comp->getOwner()->getTransformChangedEvent().addListener(m_task);
        }
        m_comp = nullptr;
        m_task = nullptr;
    }

    Inspector::Inspector(const std::string &name, const Panel::Settings &settings)
        : Panel(name, settings)
    {
    }

    Inspector::~Inspector()
    {
        clear();
    }

    void Inspector::initialize()
    {
        clear();

        if (Editor::getCurrentScene() == nullptr)
            return;

        m_inspectorEditor = std::make_shared<InspectorEditor>();
        m_targetObject = Editor::getCurrentScene()->getTarget().get();
        m_headerGroup = createWidget<Group>("Inspector_Header", false);

        // Object info
        auto columns = m_headerGroup->createWidget<Columns<2>>();
        columns->createWidget<TextField>("ID", std::to_string(m_targetObject->getId()).c_str(), true);
        columns->createWidget<CheckBox>("Active", m_targetObject->isActive())->getOnDataChangedEvent().addListener([this](bool active) {
            if (m_targetObject)
                m_targetObject->setActive(active);
        });
        m_headerGroup->createWidget<TextField>("UUID", m_targetObject->getUUID().c_str(), true);
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

            if (!m_targetObject->hasComponent<BoneTransform>())
                m_createCompCombo->addChoice((int)ComponentType::BoneTransform, "BoneTransform");

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
            m_createCompCombo->addChoice((int)ComponentType::UIButton, "UIButton");
            m_createCompCombo->addChoice((int)ComponentType::UIButton, "UISlider");
            m_createCompCombo->addChoice((int)ComponentType::UIButton, "UIScrollView");
            m_createCompCombo->addChoice((int)ComponentType::UIButton, "UIScrollBar");
            m_createCompCombo->addChoice((int)ComponentType::UIButton, "UIMask");
        }

        // Script component
        m_createCompCombo->addChoice((int)ComponentType::Script, "Script");

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
            TaskManager::getInstance()->addTask([this]() {
                switch (m_createCompCombo->getSelectedIndex())
                {
                case (int)ComponentType::Camera:
                    m_targetObject->addComponent<CameraComponent>("camera");
                    if (!m_targetObject->getComponent<FigureComponent>())
                        m_targetObject->addComponent<FigureComponent>(GetEnginePath("figures/camera.pyxf"))->setSkipSerialize(true);
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
                        m_targetObject->addComponent<SpriteComponent>(GetEnginePath("sprites/direct-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    break;
                case (int)ComponentType::PointLight:
                    m_targetObject->addComponent<PointLight>();
                    if (!m_targetObject->getComponent<FigureComponent>() && !m_targetObject->getComponent<SpriteComponent>())
                        m_targetObject->addComponent<SpriteComponent>(GetEnginePath("sprites/point-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    break;
                case (int)ComponentType::SpotLight:
                    m_targetObject->addComponent<SpotLight>();
                    if (!m_targetObject->getComponent<FigureComponent>() && !m_targetObject->getComponent<SpriteComponent>())
                        m_targetObject->addComponent<SpriteComponent>(GetEnginePath("sprites/spot-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    break;
                case (int)ComponentType::Figure:
                    m_targetObject->addComponent<FigureComponent>();
                    break;
                case (int)ComponentType::Sprite:
                    m_targetObject->addComponent<SpriteComponent>();
                    break;
                case (int)ComponentType::BoneTransform:
                    m_targetObject->addComponent<BoneTransform>()->initialize();
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
                case (int)ComponentType::UIButton:
                    m_targetObject->addComponent<UIButton>();
                    break;
                case (int)ComponentType::UISlider:
                    m_targetObject->addComponent<UISlider>();
                    break;
                case (int)ComponentType::UIScrollView:
                    m_targetObject->addComponent<UIScrollView>();
                    break;
                case (int)ComponentType::UIScrollBar:
                    m_targetObject->addComponent<UIScrollBar>();
                    break;
                case (int)ComponentType::UIMask:
                    m_targetObject->addComponent<UIMask>();
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
        });

        // Component
        createWidget<Separator>();
        m_componentGroup = createWidget<Group>("Inspector_Components", false);
        for(auto comp: m_targetObject->getComponents()) {
            auto component = std::dynamic_pointer_cast<CompoundComponent>(comp)->getComponents()[0];
            auto componentName = component->getName();
            auto componentId = component->getInstanceId();
            auto closable = (componentName != "Transform" && componentName != "RectTransform");
            auto header = m_componentGroup->createWidget<Group>(componentName, true, closable);
            header->getOnClosedEvent().addListener([this, componentName, componentId]() {
                m_inspectorEditor->removeComponent(componentId);
                m_targetObject->removeComponent(componentName);
                redraw();
            });

            if (componentName == "Transform")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Transform, component, header);
            }
            else if (componentName == "BoneTransform")
            {
                m_inspectorEditor->addComponent((int)ComponentType::BoneTransform, component, header);
            }
            else if (componentName == "Camera")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Camera, component, header);
            }
            else if (componentName == "Environment")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Environment, component, header);
            }
            else if (componentName == "Figure")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Figure, component, header);
            }
            else if (componentName == "Sprite")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Sprite, component, header);
            }
            else if (componentName == "Script")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Script, component, header);
            }
            else if (componentName == "RectTransform")
            {
                m_inspectorEditor->addComponent((int)ComponentType::RectTransform, component, header);
            }
            else if (componentName == "Canvas")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Canvas, component, header);
            }
            else if (componentName == "UIImage")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIImage, component, header);
            }
            else if (componentName == "UIText" )
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIText, component, header);
            }
            else if (componentName == "UITextField")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UITextField, component, header);
            }
            else if (componentName == "UIButton")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIButton, component, header);
            }
            else if (componentName == "UISlider")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UISlider, component, header);
            }
            else if (componentName == "UIScrollView")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIScrollView, component, header);
            }
            else if (componentName == "UIScrollBar")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIScrollBar, component, header);
            }
            else if (componentName == "UIMask")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIMask, component, header);
            }
            else if (componentName == "PhysicManager")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicManager, component, header);
            }
            else if (componentName == "PhysicBox")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicBox, component, header);
            }
            else if (componentName == "PhysicSphere")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicSphere, component, header);
            }
            else if (componentName == "PhysicCapsule")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicCapsule, component, header);
            }
            else if (componentName == "PhysicMesh")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicMesh, component, header);
            }
            else if (componentName == "PhysicSoftBody")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicSoftBody, component, header);
            }
            else if (componentName == "AudioManager")
            {
                m_inspectorEditor->addComponent((int)ComponentType::AudioManager, component, header);
            }
            else if (componentName == "AudioSource")
            {
                m_inspectorEditor->addComponent((int)ComponentType::AudioSource, component, header);
            }
            else if (componentName == "AudioListener")
            {
                m_inspectorEditor->addComponent((int)ComponentType::AudioListener, component, header);
            }
            else if (componentName == "AmbientLight")
            {
                m_inspectorEditor->addComponent((int)ComponentType::AmbientLight, component, header);
            }
            else if (componentName == "DirectionalLight")
            {
                m_inspectorEditor->addComponent((int)ComponentType::DirectionalLight, component, header);
            }
            else if (componentName == "PointLight")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PointLight, component, header);
            }
            else if (componentName == "SpotLight")
            {
                m_inspectorEditor->addComponent((int)ComponentType::SpotLight, component, header);
            }
            else if (componentName == "ParticleManager")
            {
                m_inspectorEditor->addComponent((int)ComponentType::ParticleManager, component, header);
            }
            else if (componentName == "Particle")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Particle, component, header);
            }
            else if (componentName == "Navigable")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Navigable, component, header);
            }
            else if (componentName == "NavMesh")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavMesh, component, header);
            }
            else if (componentName == "NavAgent")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavAgent, component, header);
            }
            else if (componentName == "NavAgentManager")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavAgentManager, component, header);
            }        
            else if (componentName == "DynamicNavMesh")
            {
                m_inspectorEditor->addComponent((int)ComponentType::DynamicNavMesh, component, header);
            }        
            else if (componentName == "NavObstacle")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavObstacle, component, header);
            }  
            else if (componentName == "NavArea")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavArea, component, header);
            }
            else if (componentName == "OffMeshLink")
            {
                m_inspectorEditor->addComponent((int)ComponentType::OffMeshLink, component, header);
            }
        }
    }

    void Inspector::update(float dt)
    {
        Panel::update(dt);
        if (m_inspectorEditor)
            m_inspectorEditor->update(dt);
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
        m_targetObject = nullptr;
        m_inspectorEditor = nullptr;

        m_createCompCombo = nullptr;
        m_headerGroup = nullptr;
        m_componentGroup = nullptr;

        removeAllWidgets();
    }

} // namespace ige::creator
