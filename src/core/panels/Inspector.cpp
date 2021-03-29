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
#include "core/Canvas.h"
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
#include <components/gui/UIButton.h>
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
using namespace ige::scene;

#include <pyxieUtilities.h>
using namespace pyxie;

namespace ige::creator
{
    IgnoreTransformEventScope::IgnoreTransformEventScope(std::shared_ptr<SceneObject> obj, const std::function<void(SceneObject&)>& task)
        : m_object(obj), m_task(task)
    {
        auto listenerId = Editor::getInstance()->getCanvas()->getInspector()->getTransformListenerId();
        if (listenerId != (uint64_t)-1)
            m_object->getTransformChangedEvent().removeListener(listenerId);
    }

    IgnoreTransformEventScope::~IgnoreTransformEventScope()
    {
        auto listenerId = m_object->getTransformChangedEvent().addListener(m_task);
        Editor::getInstance()->getCanvas()->getInspector()->setTransformListenerId(listenerId);
        m_object = nullptr;
        m_task = nullptr;
    }

    Inspector::Inspector(const std::string &name, const Panel::Settings &settings)
        : Panel(name, settings)
    {
        m_inspectorEditor = std::make_shared<InspectorEditor>();
    }

    Inspector::~Inspector()
    {
        m_targetObject = nullptr;

        clear();

        m_inspectorEditor->clear();
        m_inspectorEditor = nullptr;
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
            m_createCompCombo->addChoice((int)ComponentType::UIButton, "UIButton");
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
            TaskManager::getInstance()->addTask([this]() {
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
                case (int)ComponentType::UIButton:
                    m_targetObject->addComponent<UIButton>();
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
        m_headerGroup->createWidget<Separator>();
        m_componentGroup = createWidget<Group>("Inspector_Components", false);
        m_inspectorEditor->setParentGroup(m_componentGroup);
        std::for_each(m_targetObject->getComponents().begin(), m_targetObject->getComponents().end(), [this](auto &component) {
            auto closable = (component->getName() != "TransformComponent" && component->getName() != "RectTransform");
            auto header = m_componentGroup->createWidget<Group>(component->getName(), true, closable);
            header->getOnClosedEvent().addListener([this, &component]() {
                m_inspectorEditor->removeComponent(component->getInstanceId());
                m_targetObject->removeComponent(component);
                redraw();
            });

            if (component->getName() == "TransformComponent")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Transform, component.get(), header);
            }
            else if (component->getName() == "CameraComponent")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Camera, component.get(), header);
            }
            else if (component->getName() == "EnvironmentComponent")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Environment, component.get(), header);
            }
            else if (component->getName() == "FigureComponent")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Figure, component.get(), header);
            }
            else if (component->getName() == "SpriteComponent")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Sprite, component.get(), header);
            }
            else if (component->getName() == "ScriptComponent")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Script, component.get(), header);
            }
            else if (component->getName() == "RectTransform")
            {
                m_inspectorEditor->addComponent((int)ComponentType::RectTransform, component.get(), header);
            }
            else if (component->getName() == "Canvas")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Canvas, component.get(), header);
            }
            else if (component->getName() == "UIImage")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIImage, component.get(), header);
            }
            else if (component->getName() == "UIText" )
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIText, component.get(), header);
            }
            else if (component->getName() == "UITextField")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UITextField, component.get(), header);
            }
            else if (component->getName() == "UIButton")
            {
                m_inspectorEditor->addComponent((int)ComponentType::UIButton, component.get(), header);
            }
            else if (component->getName() == "PhysicManager")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicManager, component.get(), header);
            }
            else if (component->getName() == "PhysicBox")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicBox, component.get(), header);
            }
            else if (component->getName() == "PhysicSphere")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicSphere, component.get(), header);
            }
            else if (component->getName() == "PhysicCapsule")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicCapsule, component.get(), header);
            }
            else if (component->getName() == "PhysicMesh")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicMesh, component.get(), header);
            }
            else if (component->getName() == "PhysicSoftBody")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PhysicSoftBody, component.get(), header);
            }
            else if (component->getName() == "AudioManager")
            {
                m_inspectorEditor->addComponent((int)ComponentType::AudioManager, component.get(), header);
            }
            else if (component->getName() == "AudioSource")
            {
                m_inspectorEditor->addComponent((int)ComponentType::AudioSource, component.get(), header);
            }
            else if (component->getName() == "AudioListener")
            {
                m_inspectorEditor->addComponent((int)ComponentType::AudioListener, component.get(), header);
            }
            else if (component->getName() == "AmbientLight")
            {
                m_inspectorEditor->addComponent((int)ComponentType::AmbientLight, component.get(), header);
            }
            else if (component->getName() == "DirectionalLight")
            {
                m_inspectorEditor->addComponent((int)ComponentType::DirectionalLight, component.get(), header);
            }
            else if (component->getName() == "PointLight")
            {
                m_inspectorEditor->addComponent((int)ComponentType::PointLight, component.get(), header);
            }
            else if (component->getName() == "SpotLight")
            {
                m_inspectorEditor->addComponent((int)ComponentType::SpotLight, component.get(), header);
            }
            else if (component->getName() == "ParticleManager")
            {
                m_inspectorEditor->addComponent((int)ComponentType::ParticleManager, component.get(), header);
            }
            else if (component->getName() == "Particle")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Particle, component.get(), header);
            }
            else if (component->getName() == "Navigable")
            {
                m_inspectorEditor->addComponent((int)ComponentType::Navigable, component.get(), header);
            }
            else if (component->getName() == "NavMesh")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavMesh, component.get(), header);
            }
            else if (component->getName() == "NavAgent")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavAgent, component.get(), header);
            }
            else if (component->getName() == "NavAgentManager")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavAgentManager, component.get(), header);
            }        
            else if (component->getName() == "DynamicNavMesh")
            {
                m_inspectorEditor->addComponent((int)ComponentType::DynamicNavMesh, component.get(), header);
            }        
            else if (component->getName() == "NavObstacle")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavObstacle, component.get(), header);
            }  
            else if (component->getName() == "NavArea")
            {
                m_inspectorEditor->addComponent((int)ComponentType::NavArea, component.get(), header);
            }
            else if (component->getName() == "OffMeshLink")
            {
                m_inspectorEditor->addComponent((int)ComponentType::OffMeshLink, component.get(), header);
            }
        });
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
                
        removeAllWidgets();
    }

    void Inspector::setTargetObject(const std::shared_ptr<SceneObject> &obj)
    {
        if (m_targetObject != obj)
        {
            if (m_targetObject && m_transformListenerId != (uint64_t)-1)
                m_targetObject->getTransformChangedEvent().removeListener(m_transformListenerId);
            m_targetObject = obj;

            clear();
            m_inspectorEditor->clear();
            m_inspectorEditor->setTargetObject(m_targetObject);
            if (m_targetObject != nullptr)
            {
                initialize();
            }
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
