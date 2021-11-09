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
#include "core/FileHandle.h"
#include "core/task/TaskManager.h"
#include "core/scene/CompoundComponent.h"
#include "core/scene/TargetObject.h"

#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"
#include "core/panels/InspectorEditor.h"

#include <components/CameraComponent.h>
#include <components/TransformComponent.h>
#include <components/EnvironmentComponent.h>
#include <components/FigureComponent.h>
#include <components/EditableFigureComponent.h>
#include <components/BoneTransform.h>
#include <components/SpriteComponent.h>
#include <components/TextComponent.h>
#include <components/TextBitmapComponent.h>
#include <components/ScriptComponent.h>
#include <components/light/AmbientLight.h>
#include <components/light/DirectionalLight.h>
#include <components/light/PointLight.h>
#include <components/light/SpotLight.h>
#include <components/gui/RectTransform.h>
#include <components/gui/Canvas.h>
#include <components/gui/UIImage.h>
#include <components/gui/UIText.h>
#include <components/gui/UITextBitmap.h>
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
            m_createCompCombo->addChoice((int)Component::Type::UITextBitmap, "UITextBitmap");
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
            m_createCompCombo->addChoice((int)Component::Type::Sprite, "Sprite");
            m_createCompCombo->addChoice((int)Component::Type::Text, "Text");
            m_createCompCombo->addChoice((int)Component::Type::TextBitmap, "TextBitmap");

            if (!m_targetObject->hasComponent<BoneTransform>())
                m_createCompCombo->addChoice((int)Component::Type::BoneTransform, "BoneTransform");

            if (m_targetObject->getComponent<PhysicObject>() == nullptr && m_targetObject->getComponent<PhysicSoftBody>() == nullptr)
            {
                m_createCompCombo->addChoice((int)Component::Type::PhysicBox, "PhysicBox");
                m_createCompCombo->addChoice((int)Component::Type::PhysicSphere, "PhysicSphere");
                m_createCompCombo->addChoice((int)Component::Type::PhysicCapsule, "PhysicCapsule");
                m_createCompCombo->addChoice((int)Component::Type::PhysicMesh, "PhysicMesh");
                if (m_targetObject->getComponent<FigureComponent>())
                    m_createCompCombo->addChoice((int)Component::Type::PhysicSoftBody, "PhysicSoftBody");
            }

            // Particle
            m_createCompCombo->addChoice((int)Component::Type::Particle, "Particle");

            // Navigation
            if (!m_targetObject->getComponent<NavMesh>() && !m_targetObject->getComponent<DynamicNavMesh>())
            {
                m_createCompCombo->addChoice((int)Component::Type::NavMesh, "NavMesh");
                m_createCompCombo->addChoice((int)Component::Type::DynamicNavMesh, "DynamicNavMesh");
            }
            m_createCompCombo->addChoice((int)Component::Type::Navigable, "Navigable");
            m_createCompCombo->addChoice((int)Component::Type::NavAgent, "NavAgent");
            m_createCompCombo->addChoice((int)Component::Type::NavObstacle, "NavObstacle");
            m_createCompCombo->addChoice((int)Component::Type::OffMeshLink, "OffMeshLink");
        }

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
                    m_targetObject->addComponent<CameraComponent>("camera");
                    if (!m_targetObject->getComponent<FigureComponent>())
                        m_targetObject->addComponent<FigureComponent>(GetEnginePath("figures/camera"))->setSkipSerialize(true);
                    break;
                case (int)Component::Type::Environment:
                    m_targetObject->addComponent<EnvironmentComponent>();
                    break;
                case (int)Component::Type::Script:
                    m_targetObject->addComponent<ScriptComponent>();
                    break;
                case (int)Component::Type::AmbientLight:
                    m_targetObject->addComponent<AmbientLight>();
                    break;
                case (int)Component::Type::DirectionalLight:
                    m_targetObject->addComponent<DirectionalLight>();
                    if (!m_targetObject->getComponent<FigureComponent>() && !m_targetObject->getComponent<SpriteComponent>())
                        m_targetObject->addComponent<SpriteComponent>(GetEnginePath("sprites/direct-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    break;
                case (int)Component::Type::PointLight:
                    m_targetObject->addComponent<PointLight>();
                    if (!m_targetObject->getComponent<FigureComponent>() && !m_targetObject->getComponent<SpriteComponent>())
                        m_targetObject->addComponent<SpriteComponent>(GetEnginePath("sprites/point-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    break;
                case (int)Component::Type::SpotLight:
                    m_targetObject->addComponent<SpotLight>();
                    if (!m_targetObject->getComponent<FigureComponent>() && !m_targetObject->getComponent<SpriteComponent>())
                        m_targetObject->addComponent<SpriteComponent>(GetEnginePath("sprites/spot-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    break;
                case (int)Component::Type::Figure:
                    m_targetObject->addComponent<FigureComponent>();
                    break;
                case (int)Component::Type::EditableFigure:
                    m_targetObject->addComponent<EditableFigureComponent>();
                    break;
                case (int)Component::Type::Sprite:
                    m_targetObject->addComponent<SpriteComponent>();
                    break;
                case (int)Component::Type::Text:
                    m_targetObject->addComponent<TextComponent>("Text", "fonts/Manjari-Regular.ttf");
                    break;
                case (int)Component::Type::TextBitmap:
                    m_targetObject->addComponent<TextBitmapComponent>("Text");
                    break;
                case (int)Component::Type::BoneTransform:
                    m_targetObject->addComponent<BoneTransform>();
                    break;
                case (int)Component::Type::UIImage:
                    m_targetObject->addComponent<UIImage>();
                    break;
                case (int)Component::Type::UIText:
                    m_targetObject->addComponent<UIText>("Text", "fonts/Manjari-Regular.ttf");
                    break;
                case (int)Component::Type::UITextBitmap:
                    m_targetObject->addComponent<UITextBitmap>("Text");
                    break;
                case (int)Component::Type::UITextField:
                    m_targetObject->addComponent<UITextField>("TextField");
                    break;
                case (int)Component::Type::UIButton:
                    m_targetObject->addComponent<UIButton>();
                    break;
                case (int)Component::Type::UISlider:
                    m_targetObject->addComponent<UISlider>();
                    break;
                case (int)Component::Type::UIScrollView:
                    m_targetObject->addComponent<UIScrollView>();
                    break;
                case (int)Component::Type::UIScrollBar:
                    m_targetObject->addComponent<UIScrollBar>();
                    break;
                case (int)Component::Type::UIMask:
                    m_targetObject->addComponent<UIMask>();
                    break;
                case (int)Component::Type::PhysicBox:
                    m_targetObject->addComponent<PhysicBox>();
                    break;
                case (int)Component::Type::PhysicSphere:
                    m_targetObject->addComponent<PhysicSphere>();
                    break;
                case (int)Component::Type::PhysicCapsule:
                    m_targetObject->addComponent<PhysicCapsule>();
                    break;
                case (int)Component::Type::PhysicMesh:
                    m_targetObject->addComponent<PhysicMesh>();
                    break;
                case (int)Component::Type::PhysicSoftBody:
                    m_targetObject->addComponent<PhysicSoftBody>();
                    break;
                case (int)Component::Type::AudioSource:
                    m_targetObject->addComponent<AudioSource>();
                    break;
                case (int)Component::Type::AudioListener:
                    m_targetObject->addComponent<AudioListener>();
                    break;
                case (int)Component::Type::Particle:
                    m_targetObject->addComponent<Particle>();
                    break;
                case (int)Component::Type::Navigable:
                    m_targetObject->addComponent<Navigable>();
                    break;
                case (int)Component::Type::NavMesh:
                    m_targetObject->addComponent<NavMesh>();
                    break;
                case (int)Component::Type::NavAgent:
                    m_targetObject->addComponent<NavAgent>();
                    break;
                case (int)Component::Type::DynamicNavMesh:
                    m_targetObject->addComponent<DynamicNavMesh>();
                    break;
                case (int)Component::Type::NavObstacle:
                    m_targetObject->addComponent<NavObstacle>();
                    break;
                case (int)Component::Type::OffMeshLink:
                    m_targetObject->addComponent<OffMeshLink>();
                    break;
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
                m_targetObject->removeComponent(compId);
                redraw();
            });
            m_inspectorEditor->addComponent((int)component->getType(), component, header);
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
