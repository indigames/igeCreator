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
#include <components/physic/collider/BoxCollider.h>
#include <components/physic/collider/SphereCollider.h>
#include <components/physic/collider/CapsuleCollider.h>
#include <components/physic/collider/CompoundCollider.h>
#include <components/physic/collider/MeshCollider.h>
#include <components/physic/Softbody.h>
#include <components/physic/constraint/FixedConstraint.h>
#include <components/physic/constraint/HingeConstraint.h>
#include <components/physic/constraint/SliderConstraint.h>
#include <components/physic/constraint/SpringConstraint.h>
#include <components/physic/constraint/Dof6SpringConstraint.h>
#include <components/audio/AudioManager.h>
#include <components/audio/AudioSource.h>
#include <components/audio/AudioListener.h>
#include <components/particle/ParticleManager.h>
#include <components/particle/Particle.h>
#include <components/navigation/NavMesh.h>
#include <components/navigation/NavAgent.h>
#include <components/navigation/NavAgentManager.h>
#include <components/navigation/Navigable.h>
#include <components/navigation/NavArea.h>
#include <components/navigation/DynamicNavMesh.h>
#include <components/navigation/NavObstacle.h>
#include <components/navigation/OffMeshLink.h>
#include <components/animation/AnimatorComponent.h>

#include <scene/Scene.h>
#include <utils/GraphicsHelper.h>
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

        // Object info
        _drawBaseInfo();

        // Components
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
        auto createCompButton = m_componentGroup->createWidget<Button>("Add Component", ImVec2(128.f, 0.f));
        createCompButton->getOnClickEvent().addListener([this](auto widget) {
            auto firstTarget = std::weak_ptr<SceneObject>(Editor::getInstance()->getTarget()->getFirstTarget());
            auto ctx = m_headerGroup->getPlugin<ContextMenu>();
            if (ctx == nullptr) ctx = m_headerGroup->addPlugin<ContextMenu>("Component_Add_Menu");
            else ctx->removeAllWidgets();
            ctx->open();

            // GUI
            if (!firstTarget.expired() && firstTarget.lock()->isGUIObject()) {
                auto guiMenu = ctx->createWidget<Menu>("GUI");
                guiMenu->createWidget<MenuItem>("UIImage")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<UIImage>();
                    onAddComponent(comp);
                });
                guiMenu->createWidget<MenuItem>("UIMask")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<UIMask>();
                    onAddComponent(comp);
                });
                guiMenu->createWidget<MenuItem>("UIText")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<UIText>("Text");
                    onAddComponent(comp);
                    });
                guiMenu->createWidget<MenuItem>("UITextField")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<UITextField>("TextField");
                    onAddComponent(comp);
                });
                guiMenu->createWidget<MenuItem>("UIButton")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<UIButton>();
                    onAddComponent(comp);
                });
                guiMenu->createWidget<MenuItem>("UISlider")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<UISlider>();
                    onAddComponent(comp);
                });
                guiMenu->createWidget<MenuItem>("UIScrollView")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<UIScrollView>();
                    onAddComponent(comp);
                });
                guiMenu->createWidget<MenuItem>("UIScrollBar")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<UIScrollBar>();
                    onAddComponent(comp);
                });
            }
            else
            {
                // Graphics
                auto graphicMenu = ctx->createWidget<Menu>("Graphics");
                graphicMenu->createWidget<MenuItem>("Camera")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<CameraComponent>(GraphicsHelper::unique("camera"));
                    onAddComponent(comp);
                });
                graphicMenu->createWidget<MenuItem>("Figure")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<FigureComponent>();
                    onAddComponent(comp);
                });
                graphicMenu->createWidget<MenuItem>("EditableFigure")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<EditableFigureComponent>();
                    onAddComponent(comp);
                });
                graphicMenu->createWidget<MenuItem>("Animator")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<AnimatorComponent>();
                    onAddComponent(comp);
                });
                graphicMenu->createWidget<MenuItem>("Sprite")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<SpriteComponent>();
                    onAddComponent(comp);
                });
                graphicMenu->createWidget<MenuItem>("Text")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<TextComponent>("Text");
                    onAddComponent(comp);
                });

                // Lights
                auto lightMenu = ctx->createWidget<Menu>("Lights");
                if (m_targetObject->getScene()->isDirectionalLightAvailable()) {
                    lightMenu->createWidget<MenuItem>("DirectionalLight")->getOnClickEvent().addListener([this](auto widget) {
                        auto comp = m_targetObject->addComponent<DirectionalLight>();
                        onAddComponent(comp);
                    });
                }
                if (m_targetObject->getScene()->isPointLightAvailable()) {
                    lightMenu->createWidget<MenuItem>("PointLight")->getOnClickEvent().addListener([this](auto widget) {
                        auto comp = m_targetObject->addComponent<PointLight>();
                        onAddComponent(comp);
                    });
                }
                if (m_targetObject->getScene()->isSpotLightAvailable()) {
                    lightMenu->createWidget<MenuItem>("SpotLight")->getOnClickEvent().addListener([this](auto widget) {
                        auto comp = m_targetObject->addComponent<SpotLight>();
                        onAddComponent(comp);
                    });
                }

                // Audio
                auto audioMenu = ctx->createWidget<Menu>("Audio");
                audioMenu->createWidget<MenuItem>("AudioSource")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<AudioSource>();
                    onAddComponent(comp);
                });
                audioMenu->createWidget<MenuItem>("AudioListener")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<AudioListener>();
                    onAddComponent(comp);
                });

                // Physic
                auto physicMenu = ctx->createWidget<Menu>("Physic");
                if (!firstTarget.expired()) {
                    if (firstTarget.lock()->getComponent<Rigidbody>() == nullptr && firstTarget.lock()->getComponent<Softbody>() == nullptr && firstTarget.lock()->getComponent<Collider>() != nullptr) {
                        physicMenu->createWidget<MenuItem>("Rigidbody")->getOnClickEvent().addListener([this](auto widget) {
                            auto comp = m_targetObject->addComponent<Rigidbody>();
                            onAddComponent(comp);
                            auto* target = dynamic_cast<TargetObject*>(m_targetObject);
                            if (target) target->getFirstTarget()->getComponent<Rigidbody>()->init();
                        });
                    }
                    if (firstTarget.lock()->getComponent<Collider>() == nullptr && firstTarget.lock()->getComponent<Softbody>() == nullptr)
                    {
                        physicMenu->createWidget<MenuItem>("BoxCollider")->getOnClickEvent().addListener([this](auto widget) {
                            auto comp = m_targetObject->addComponent<BoxCollider>();
                            onAddComponent(comp);
                            auto* target = dynamic_cast<TargetObject*>(m_targetObject);
                            if (target) target->getFirstTarget()->getComponent<BoxCollider>()->init();
                        });
                        physicMenu->createWidget<MenuItem>("SphereCollider")->getOnClickEvent().addListener([this](auto widget) {
                            auto comp = m_targetObject->addComponent<SphereCollider>();
                            onAddComponent(comp);
                            auto* target = dynamic_cast<TargetObject*>(m_targetObject);
                            if (target) target->getFirstTarget()->getComponent<SphereCollider>()->init();
                        });
                        physicMenu->createWidget<MenuItem>("CapsuleCollider")->getOnClickEvent().addListener([this](auto widget) {
                            auto comp = m_targetObject->addComponent<CapsuleCollider>();
                            onAddComponent(comp);
                            auto* target = dynamic_cast<TargetObject*>(m_targetObject);
                            if (target) target->getFirstTarget()->getComponent<CapsuleCollider>()->init();
                        });
                        physicMenu->createWidget<MenuItem>("CompoundCollider")->getOnClickEvent().addListener([this](auto widget) {
                            auto comp = m_targetObject->addComponent<CompoundCollider>();
                            onAddComponent(comp);
                            auto* target = dynamic_cast<TargetObject*>(m_targetObject);
                            if (target) target->getFirstTarget()->getComponent<CompoundCollider>()->init();
                        });

                        // Mesh based physic
                        if (firstTarget.lock()->getComponent<FigureComponent>() || firstTarget.lock()->getComponent<EditableFigureComponent>()) {
                            physicMenu->createWidget<MenuItem>("MeshCollider")->getOnClickEvent().addListener([this](auto widget) {
                                auto comp = m_targetObject->addComponent<MeshCollider>();
                                onAddComponent(comp);
                                auto* target = dynamic_cast<TargetObject*>(m_targetObject);
                                if (target) target->getFirstTarget()->getComponent<MeshCollider>()->init();
                            });
                            physicMenu->createWidget<MenuItem>("Softbody")->getOnClickEvent().addListener([this](auto widget) {
                                auto comp = m_targetObject->addComponent<Softbody>();
                                onAddComponent(comp);
                                auto* target = dynamic_cast<TargetObject*>(m_targetObject);
                                if (target) target->getFirstTarget()->getComponent<Softbody>()->init();
                            });
                        }
                    }
                }

                // Navigation
                auto navMenu = ctx->createWidget<Menu>("Navigation");
                if (!firstTarget.expired() && !firstTarget.lock()->getComponent<NavMesh>() && !firstTarget.lock()->getComponent<DynamicNavMesh>())
                {
                    navMenu->createWidget<MenuItem>("NavMesh")->getOnClickEvent().addListener([this](auto widget) {
                        auto comp = m_targetObject->addComponent<NavMesh>();
                        onAddComponent(comp);
                    });
                    navMenu->createWidget<MenuItem>("DynamicNavMesh")->getOnClickEvent().addListener([this](auto widget) {
                        auto comp = m_targetObject->addComponent<DynamicNavMesh>();
                        onAddComponent(comp);
                    });
                }
                navMenu->createWidget<MenuItem>("Navigable")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<Navigable>();
                    onAddComponent(comp);
                });
                navMenu->createWidget<MenuItem>("NavArea")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<NavArea>();
                    onAddComponent(comp);
                });
                navMenu->createWidget<MenuItem>("NavAgent")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<NavAgent>();
                    onAddComponent(comp);
                });
                navMenu->createWidget<MenuItem>("NavObstacle")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<NavObstacle>();
                    onAddComponent(comp);
                });
                navMenu->createWidget<MenuItem>("OffMeshLink")->getOnClickEvent().addListener([this](auto widget) {
                    auto comp = m_targetObject->addComponent<OffMeshLink>();
                    onAddComponent(comp);
                });

                // BoneTransform
                if (!firstTarget.expired() && !firstTarget.lock()->hasComponent<BoneTransform>()) {
                    ctx->createWidget<MenuItem>("BoneTransform")->getOnClickEvent().addListener([this](auto widget) {
                        auto comp = m_targetObject->addComponent<BoneTransform>();
                        onAddComponent(comp);
                    });
                }
            }

            // Particle
            ctx->createWidget<MenuItem>("Particle")->getOnClickEvent().addListener([this](auto widget) {
                auto comp = m_targetObject->addComponent<Particle>();
                onAddComponent(comp);
            });

            // Script
            ctx->createWidget<MenuItem>("Script")->getOnClickEvent().addListener([this](auto widget) {
                auto comp = m_targetObject->addComponent<ScriptComponent>();
                onAddComponent(comp);
            });
         });
    }

    void Inspector::onAddComponent(std::shared_ptr<Component> comp)
    {
        json j = json{};
        auto jComponents = json::array();
        jComponents.push_back({ comp->getName(), json(*comp.get()) });
        j["comps"] = jComponents;
        CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_COMPONENT, Editor::getInstance()->getTarget(), j);
        redraw();
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
        m_infoGroup = m_headerGroup->createWidget<Group>("BaseInfo_Header", false);

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
