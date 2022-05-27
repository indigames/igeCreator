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
#include "core/panels/AnimatorEditor.h"
#include "core/FileHandle.h"
#include "core/task/TaskManager.h"
#include "core/scene/CompoundComponent.h"
#include "core/scene/TargetObject.h"

#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"
#include "core/panels/InspectorEditor.h"

#include "core/CommandManager.h"

#include <components/CameraComponent.h>
#include <components/TransformComponent.h>
#include <components/EnvironmentComponent.h>
#include <components/FigureComponent.h>
#include <components/EditableFigureComponent.h>
#include <components/BoneTransform.h>
#include <components/SpriteComponent.h>
#include <components/TextComponent.h>
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
#include <components/physic/BoxCollider.h>
#include <components/physic/SphereCollider.h>
#include <components/physic/CapsuleCollider.h>
#include <components/physic/MeshCollider.h>
#include <components/physic/Softbody.h>
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
#include <components/animation/AnimatorComponent.h>

#include <scene/Scene.h>
using namespace ige::scene;

#include <pyxieUtilities.h>
using namespace pyxie;

#include <imgui.h>

namespace ige::creator
{
    IgnoreTransformEventScope::IgnoreTransformEventScope(SceneObject* obj, uint64_t& eventId, const std::function<void(SceneObject&)>& task)
        : m_obj(obj), m_eventId(eventId), m_task(task)
    {
        if (m_obj && m_eventId != (uint64_t)-1)
            m_obj->getTransformChangedEvent().removeListener(m_eventId);

    }

    IgnoreTransformEventScope::~IgnoreTransformEventScope()
    {
        if (m_obj)
            m_eventId = m_obj->getTransformChangedEvent().addListener(m_task);        
        m_obj = nullptr;
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

        if (!Editor::getInstance()->getTarget() || Editor::getInstance()->getTarget()->size() == 0)
            return;

        m_inspectorEditor = std::make_shared<InspectorEditor>();
        m_targetObject = Editor::getInstance()->getTarget().get();
        m_headerGroup = createWidget<Group>("Inspector_Header", false);
        auto* firstTarget = Editor::getInstance()->getTarget()->getFirstTarget().get();

        // Object info
        _drawBaseInfo();

        // Create component selection
        m_createCompCombo = m_headerGroup->createWidget<ComboBox>("");
        m_createCompCombo->setEndOfLine(false);

        // Script component
        m_createCompCombo->addChoice((int)Component::Type::Script, "Script");

        m_createCompCombo->addChoice((int)Component::Type::Camera, "Camera");

        if(m_targetObject->getScene()->isDirectionalLightAvailable())
            m_createCompCombo->addChoice((int)Component::Type::DirectionalLight, "Directional Light");

        if (m_targetObject->getScene()->isPointLightAvailable())
            m_createCompCombo->addChoice((int)Component::Type::PointLight, "Point Light");

        if (m_targetObject->getScene()->isSpotLightAvailable())
            m_createCompCombo->addChoice((int)Component::Type::SpotLight, "Spot Light");

        // GUI Object
        if (Editor::getInstance()->getTarget()->getFirstTarget()->isGUIObject())
        {
            m_createCompCombo->addChoice((int)Component::Type::UIImage, "UIImage");
            m_createCompCombo->addChoice((int)Component::Type::UIText, "UIText");
            m_createCompCombo->addChoice((int)Component::Type::UITextField, "UITextField");
            m_createCompCombo->addChoice((int)Component::Type::UIButton, "UIButton");
            m_createCompCombo->addChoice((int)Component::Type::UIButton, "UISlider");
            m_createCompCombo->addChoice((int)Component::Type::UIButton, "UIScrollView");
            m_createCompCombo->addChoice((int)Component::Type::UIButton, "UIScrollBar");
            m_createCompCombo->addChoice((int)Component::Type::UIButton, "UIMask"); 
        }
        else // Scene Object
        {
            m_createCompCombo->addChoice((int)Component::Type::Figure, "Figure");
            m_createCompCombo->addChoice((int)Component::Type::EditableFigure, "EditableFigure");
            m_createCompCombo->addChoice((int)Component::Type::Animator, "Animator");
            m_createCompCombo->addChoice((int)Component::Type::Sprite, "Sprite");
            m_createCompCombo->addChoice((int)Component::Type::Text, "Text");
            m_createCompCombo->addChoice((int)Component::Type::TextBitmap, "TextBitmap");

            if (!firstTarget->hasComponent<BoneTransform>())
                m_createCompCombo->addChoice((int)Component::Type::BoneTransform, "BoneTransform");

            if (firstTarget->getComponent<Rigidbody>() == nullptr && firstTarget->getComponent<Softbody>() == nullptr && firstTarget->getComponent<Collider>() != nullptr) {
                m_createCompCombo->addChoice((int)Component::Type::Rigidbody, "Rigidbody");
            }

            if (firstTarget->getComponent<Collider>() == nullptr && firstTarget->getComponent<Softbody>() == nullptr)
            {
                m_createCompCombo->addChoice((int)Component::Type::BoxCollider, "BoxCollider");
                m_createCompCombo->addChoice((int)Component::Type::SphereCollider, "SphereCollider");
                m_createCompCombo->addChoice((int)Component::Type::CapsuleCollider, "CapsuleCollider");
                
                // Mesh based physic
                if (firstTarget->getComponent<FigureComponent>()) {
                    m_createCompCombo->addChoice((int)Component::Type::MeshCollider, "MeshCollider");
                    m_createCompCombo->addChoice((int)Component::Type::Softbody, "Softbody");
                }
            }

            // Navigation
            if (!firstTarget->getComponent<NavMesh>() && !firstTarget->getComponent<DynamicNavMesh>())
            {
                m_createCompCombo->addChoice((int)Component::Type::NavMesh, "NavMesh");
                m_createCompCombo->addChoice((int)Component::Type::DynamicNavMesh, "DynamicNavMesh");
            }
            m_createCompCombo->addChoice((int)Component::Type::Navigable, "Navigable");
            m_createCompCombo->addChoice((int)Component::Type::NavAgent, "NavAgent");
            m_createCompCombo->addChoice((int)Component::Type::NavObstacle, "NavObstacle");
            m_createCompCombo->addChoice((int)Component::Type::OffMeshLink, "OffMeshLink");
        }

        // Particle
        m_createCompCombo->addChoice((int)Component::Type::Particle, "Particle");

        // Audio source
        m_createCompCombo->addChoice((int)Component::Type::AudioSource, "Audio Source");

        // Audio listener
        m_createCompCombo->addChoice((int)Component::Type::AudioListener, "Audio Listener");

        auto createCompButton = m_headerGroup->createWidget<Button>("Add", ImVec2(64.f, 0.f));
        createCompButton->getOnClickEvent().addListener([this](auto widget) {
            TaskManager::getInstance()->addTask([this]() {
                switch (m_createCompCombo->getSelectedIndex())
                {
                    case (int)Component::Type::Camera:
                    {
                        std::vector<std::shared_ptr<Component>> comps;
                        comps.push_back(m_targetObject->addComponent<CameraComponent>("camera"));
                        onAddComponents(comps);
                        break;
                    }
                    case (int)Component::Type::Environment:
                    {
                        auto comp = m_targetObject->addComponent<EnvironmentComponent>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::Script:
                    {
                        auto comp = m_targetObject->addComponent<ScriptComponent>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::AmbientLight:
                    {
                        auto comp = m_targetObject->addComponent<AmbientLight>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::DirectionalLight:
                    {  
                        std::vector<std::shared_ptr<Component>> comps;
                        comps.push_back(m_targetObject->addComponent<DirectionalLight>());
                        onAddComponents(comps);
                        break;
                    }
                    case (int)Component::Type::PointLight:
                    {
                        std::vector<std::shared_ptr<Component>> comps;
                        comps.push_back(m_targetObject->addComponent<PointLight>());
                        onAddComponents(comps);
                        break;
                    }
                    case (int)Component::Type::SpotLight:
                    {
                        std::vector<std::shared_ptr<Component>> comps;
                        comps.push_back(m_targetObject->addComponent<SpotLight>());
                        onAddComponents(comps);
                        break;
                    }
                    case (int)Component::Type::Figure:
                    {
                        auto comp = m_targetObject->addComponent<FigureComponent>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::EditableFigure:
                    {
                        auto comp = m_targetObject->addComponent<EditableFigureComponent>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::Animator:
                    {
                        m_targetObject->addComponent<AnimatorComponent>();
                        break;
                    }
                    case (int)Component::Type::Sprite:
                    {
                        auto comp = m_targetObject->addComponent<SpriteComponent>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::Text:
                    {
                        auto comp = m_targetObject->addComponent<TextComponent>("Text", "fonts/Manjari-Regular.ttf");
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::BoneTransform:
                    {
                        auto comp = m_targetObject->addComponent<BoneTransform>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::UIImage:
                    {
                        auto comp = m_targetObject->addComponent<UIImage>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::UIText:
                    {
                        auto comp = m_targetObject->addComponent<UIText>("Text", "fonts/Manjari-Regular.ttf");
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::UITextField:
                    {
                        auto comp = m_targetObject->addComponent<UITextField>("TextField");
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::UIButton:
                    {
                        auto comp = m_targetObject->addComponent<UIButton>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::UISlider:
                    {
                        auto comp = m_targetObject->addComponent<UISlider>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::UIScrollView:
                    {
                        auto comp = m_targetObject->addComponent<UIScrollView>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::UIScrollBar:
                    {
                        auto comp = m_targetObject->addComponent<UIScrollBar>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::UIMask:
                    {
                        auto comp = m_targetObject->addComponent<UIMask>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::BoxCollider:
                    {
                        auto comp = m_targetObject->addComponent<BoxCollider>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::SphereCollider:
                    {
                        auto comp = m_targetObject->addComponent<SphereCollider>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::CapsuleCollider:
                    {
                        auto comp = m_targetObject->addComponent<CapsuleCollider>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::MeshCollider:
                    {
                        auto comp = m_targetObject->addComponent<MeshCollider>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::Rigidbody:
                    {
                        auto comp  = m_targetObject->addComponent<Rigidbody>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::Softbody:
                    {
                        auto comp  = m_targetObject->addComponent<Softbody>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::AudioSource:
                    {
                        auto comp = m_targetObject->addComponent<AudioSource>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::AudioListener:
                    {
                        auto comp = m_targetObject->addComponent<AudioListener>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::Particle:
                    {
                        auto comp = m_targetObject->addComponent<Particle>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::Navigable:
                    {
                        auto comp = m_targetObject->addComponent<Navigable>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::NavMesh:
                    {
                        auto comp = m_targetObject->addComponent<NavMesh>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::NavAgent:
                    {
                        auto comp = m_targetObject->addComponent<NavAgent>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::DynamicNavMesh:
                    {
                        auto comp = m_targetObject->addComponent<DynamicNavMesh>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::NavObstacle:
                    {
                        auto comp = m_targetObject->addComponent<NavObstacle>();
                        onAddComponent(comp);
                        break;
                    }
                    case (int)Component::Type::OffMeshLink:
                    {
                        auto comp = m_targetObject->addComponent<OffMeshLink>();
                        onAddComponent(comp);
                        break;
                    }
                }
                redraw();
            });
        });

        // Component
        createWidget<Separator>();
        m_componentGroup = createWidget<Group>("Inspector_Components", false);
        for(const auto& component : m_targetObject->getComponents()) {
            auto closable = (component->getType() != Component::Type::Transform && component->getType() != Component::Type::RectTransform);
            auto header = m_componentGroup->createWidget<Group>(component->getName(), true, closable);
            auto compId = component->getInstanceId();
            header->getOnClosedEvent().addListener([this, compId]() {
                m_inspectorEditor->removeComponent(compId);
                auto comp = m_targetObject->getComponent(compId);
                if (comp) {
                    onRemoveComponent(comp);
                }
                m_targetObject->removeComponent(compId);
                redraw();
            });
            m_inspectorEditor->addComponent((int)component->getType(), component, header);
        }
    }

    void Inspector::onAddComponent(std::shared_ptr<Component> comp)
    {
        json j = json{};
        auto jComponents = json::array();
        jComponents.push_back({ comp->getName(), json(*comp.get()) });
        j["comps"] = jComponents;
        CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_COMPONENT, Editor::getInstance()->getTarget(), j);
    }

    void Inspector::onAddComponents(std::vector<std::shared_ptr<Component>> comps)
    {
        json j = json{};
        auto jComponents = json::array();
        for (const auto& comp : comps)
        {
            jComponents.push_back({ comp->getName(), json(*comp.get()) });
        }
        j["comps"] = jComponents;
        CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_COMPONENT, Editor::getInstance()->getTarget(), j);
    }

    void Inspector::onRemoveComponent(std::shared_ptr<Component> comp)
    {
        json j = json{};
        auto jComponents = json::array();
        jComponents.push_back({ comp->getName(), json(*comp.get()) });
        j["comps"] = jComponents;
        CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::DELETE_COMPONENT, Editor::getInstance()->getTarget(), j);
    }

    void Inspector::update(float dt)
    {
        Panel::update(dt);
        if (m_inspectorEditor)
            m_inspectorEditor->update(dt);
    }

    void Inspector::_drawImpl()
    {
        // Inspect animator editor first
        if (Editor::getCanvas()->getAnimatorEditor()->shouldDrawInspector()) {
            Editor::getCanvas()->getAnimatorEditor()->drawInspector();
            Panel::_drawImpl();
            return;
        }

        if (m_bNeedRedraw)
        {
            initialize();
            m_bNeedRedraw = false;
            return;
        }

        if (m_bInfoDirty)
            _drawBaseInfo();

        Panel::_drawImpl();
    }

    void Inspector::_drawBaseInfo()
    {
        if (m_infoGroup != nullptr) {
            m_infoGroup->removeAllPlugins();
            m_infoGroup->removeAllWidgets();
            m_infoGroup = nullptr;
        }
        m_infoGroup = createWidget<Group>("BaseInfo_Header", false);

        // Object info
        auto columns = m_infoGroup->createWidget<Columns<2>>();
        columns->createWidget<TextField>("ID", std::to_string(m_targetObject->getId()).c_str(), true);
        columns->createWidget<CheckBox>("Active", m_targetObject->isActive())->getOnDataChangedEvent().addListener([this](bool active) {
            if (m_targetObject)
                m_targetObject->setActive(active);
        });
        m_infoGroup->createWidget<TextField>("UUID", m_targetObject->getUUID().c_str(), true);
        m_infoGroup->createWidget<TextField>("Name", m_targetObject->getName().c_str())->getOnDataChangedEvent().addListener([this](auto name) {
            if (m_targetObject) {
                m_targetObject->setName(name);
            }
        });
        if (m_targetObject->isPrefab()) {
            m_infoGroup->createWidget<TextField>("PrefabID", m_targetObject->getPrefabId().c_str(), true);
        }
        m_bInfoDirty = false;
    }

    void Inspector::clear()
    {
        if (m_infoGroup) {
            m_infoGroup->removeAllPlugins();
            m_infoGroup->removeAllWidgets();
            m_infoGroup = nullptr;
        }

        m_targetObject = nullptr;
        m_inspectorEditor = nullptr;

        m_createCompCombo = nullptr;
        m_headerGroup = nullptr;
        m_componentGroup = nullptr;

        removeAllWidgets();
    }

} // namespace ige::creator
