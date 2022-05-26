#include "core/panels/Hierarchy.h"
#include "core/panels/Inspector.h"
#include "core/filesystem/FileSystem.h"
#include "core/FileHandle.h"
#include "core/dialog/MsgBox.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/menu/ContextMenu.h"
#include "core/menu/MenuItem.h"
#include "core/task/TaskManager.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/CommandManager.h"

#include "components/FigureComponent.h"
#include "components/SpriteComponent.h"
#include "components/ScriptComponent.h"
#include "components/light/DirectionalLight.h"
#include "components/light/PointLight.h"
#include "components/light/SpotLight.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"
#include "components/gui/UIText.h"
#include "components/gui/UITextField.h"
#include "components/gui/UIButton.h"
#include "components/gui/UISlider.h"
#include "components/gui/UIScrollView.h"
#include "components/gui/UIScrollBar.h"
#include "components/gui/UIMask.h"
#include "components/audio/AudioSource.h"
#include "components/audio/AudioListener.h"
#include "components/TextComponent.h"
#include "components/particle/Particle.h"

#include <scene/Scene.h>
#include <scene/SceneObject.h>
#include <utils/PyxieHeaders.h>
using namespace ige::scene;

#define NORMAL_COLOR {1.f, 1.f, 1.f, 1.f}
#define DISABLED_COLOR {0.5f, 0.5f, 0.5f, 1.f}
#define PREFAB_COLOR {0.f, 0.85f, 0.85f, 1.f}
#define PREFAB_DISABLED_COLOR {0.f, 0.45f, 0.45f, 1.f}

namespace ige::creator
{
    Hierarchy::Hierarchy(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        SceneObject::getCreatedEvent().addListener(std::bind(&Hierarchy::onSceneObjectCreated, this, std::placeholders::_1));
        SceneObject::getDestroyedEvent().addListener(std::bind(&Hierarchy::onSceneObjectDeleted, this, std::placeholders::_1));
        SceneObject::getAttachedEvent().addListener(std::bind(&Hierarchy::onSceneObjectAttached, this, std::placeholders::_1));
        SceneObject::getDetachedEvent().addListener(std::bind(&Hierarchy::onSceneObjectDetached, this, std::placeholders::_1));
        SceneObject::getSelectedEvent().addListener(std::bind(&Hierarchy::onSceneObjectSelected, this, std::placeholders::_1));
        SceneObject::getActivatedEvent().addListener(std::bind(&Hierarchy::onSceneObjectActivated, this, std::placeholders::_1));
        Editor::getTargetAddedEvent().addListener(std::bind(&Hierarchy::onTargetAdded, this, std::placeholders::_1));
        Editor::getTargetRemovedEvent().addListener(std::bind(&Hierarchy::onTargetRemoved, this, std::placeholders::_1));
        Editor::getTargetClearedEvent().addListener(std::bind(&Hierarchy::onTargetCleared, this));
    }

    Hierarchy::~Hierarchy()
    {
        SceneObject::getCreatedEvent().removeAllListeners();
        SceneObject::getDestroyedEvent().removeAllListeners();
        SceneObject::getAttachedEvent().removeAllListeners();
        SceneObject::getDetachedEvent().removeAllListeners();
        SceneObject::getSelectedEvent().removeAllListeners();
        SceneObject::getActivatedEvent().removeAllListeners();
        Editor::getTargetAddedEvent().removeAllListeners();
        Editor::getTargetRemovedEvent().removeAllListeners();
        Editor::getTargetClearedEvent().removeAllListeners();
        clear();
        m_groupLayout = nullptr;
    }

    void Hierarchy::onSceneObjectCreated(SceneObject& sceneObject)
    {
        if (m_groupLayout == nullptr) return;
        auto objId = sceneObject.getId();
        auto name = sceneObject.getName();
        auto isGuiObj = sceneObject.isGUIObject();
        auto node = m_groupLayout->createWidget<TreeNode>(sceneObject.getName(), false, sceneObject.getChildren().size() == 0);
        node->getOnClickEvent().addListener([objId, this](auto widget) {
            auto object = Editor::getCurrentScene()->findObjectById(objId);
            if (object)
            {
                // Right click always clear targets
                auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
                if (touch->isFingerReleased(0) && touch->getFinger(0)->getFingerId() == 2)
                {
                    Editor::getInstance()->addTarget(object, true);
                    return;
                }

                auto keyModifier = Editor::getApp()->getInputHandler()->getKeyboard()->getKeyModifier();
                if (keyModifier & (int)KeyModifier::KEY_MOD_CTRL)
                {
                    if (object->isSelected())
                        Editor::getInstance()->removeTarget(object);
                    else
                        Editor::getInstance()->addTarget(object, false);
                }
                else if (keyModifier & (int)KeyModifier::KEY_MOD_SHIFT)
                {
                    auto firstTarget = Editor::getInstance()->getFirstTarget();
                    if (firstTarget == nullptr)
                    {
                        Editor::getInstance()->addTarget(object, false);
                    }
                    else
                    {
                        auto fistIdxPair = m_objectNodeMap.find(firstTarget->getId());
                        if (fistIdxPair != m_objectNodeMap.end())
                        {
                            auto currIdxPair = m_objectNodeMap.find(object->getId());
                            if (std::distance(m_objectNodeMap.begin(), fistIdxPair) > std::distance(m_objectNodeMap.begin(), currIdxPair))
                            {
                                auto tmp = fistIdxPair;
                                fistIdxPair = currIdxPair;
                                currIdxPair = tmp;
                            }
                            for (auto it = m_objectNodeMap.begin(); it != fistIdxPair; ++it)
                            {
                                auto obj = Editor::getCurrentScene()->findObjectById(it->first);
                                if (obj) Editor::getInstance()->removeTarget(obj);
                            }
                            for (auto it = fistIdxPair; it != m_objectNodeMap.end(); ++it)
                            {
                                auto obj = Editor::getCurrentScene()->findObjectById(it->first);
                                if (obj) Editor::getInstance()->addTarget(obj, false);
                                if (it == currIdxPair) break;
                            }
                            for (auto it = currIdxPair; it != m_objectNodeMap.end(); ++it)
                            {
                                if (it == currIdxPair) continue;
                                auto obj = Editor::getCurrentScene()->findObjectById(it->first);
                                if (obj) Editor::getInstance()->removeTarget(obj);
                            }
                        }
                    }
                }
                else
                {
                    Editor::getInstance()->addTarget(object, true);
                }
            }
        });

        node->getOnDoubleClickEvent().addListener([](auto widget) {
            if (Editor::getInstance()->getCanvas()->getEditorScene()) {
                Editor::getInstance()->getCanvas()->getEditorScene()->lookSelectedObject();
            }            
        });

        auto objDDTarget = node->addPlugin<DDTargetTopBottomPlugin<uint64_t>>(EDragDropID::OBJECT);
        objDDTarget->getOnDataReceivedEvent().addListener([objId](auto id) {
            if (Editor::getInstance()->getTarget() == nullptr) return; // Scene not loaded
            bool dragObjFromTarget = Editor::getInstance()->getTarget()->findObject(id) != nullptr;
            auto parent = Editor::getCurrentScene()->findObjectById(objId);
            bool isPrefabScene = parent->getScene()->getObjects().size() > 0 && parent->getScene()->getObjects()[0]->isPrefab();
            if (!parent->isInPrefab() || isPrefabScene)
            {
                if (dragObjFromTarget) {
                    auto loop = false;
                    for (auto& target : Editor::getInstance()->getTarget()->getAllTargets()) {
                        if (!target.expired() && parent->isRelative(target.lock()->getId())) {
                            loop = true;
                            break;
                        }
                    }
                    if (!loop) {
                        std::vector<std::shared_ptr<SceneObject>> movingObjects = {};
                        for (auto& target : Editor::getInstance()->getTarget()->getAllTargets()) {
                            if (!target.expired() && (!target.lock()->getParent() || target.lock()->getParent()->isSelected() == false))
                                movingObjects.push_back(target.lock());
                        }
                        for (auto& target : movingObjects) {
                            if (target->isPrefab() || !target->isInPrefab() || isPrefabScene)
                                target->setParent(parent);
                        }
                    }
                }
                else {
                    auto object = Editor::getCurrentScene()->findObjectById(id);
                    if(!parent->isRelative(id)) {
                        if (object->isPrefab() || !object->isInPrefab() || isPrefabScene)
                            object->setParent(parent);
                    }
                }
            }
        });
        objDDTarget->getOnTopDataReceivedEvent().addListener([objId](auto id) {
            if (Editor::getCurrentScene() == nullptr) return;
            auto currObject = Editor::getCurrentScene()->findObjectById(objId);
            auto parent = currObject->getParent();
            if (parent == nullptr) return; // Skip root nodes
            bool isPrefabScene = parent->getScene()->getObjects().size() > 0 && parent->getScene()->getObjects()[0]->isPrefab();
            if (!parent->isInPrefab() || isPrefabScene) {
                auto object = Editor::getCurrentScene()->findObjectById(id);
                if (!parent->isRelative(id)) {
                    if (object->isPrefab() || !object->isInPrefab() || isPrefabScene) {
                        auto idx = parent->getChildIndex(currObject);
                        if (parent == object->getParent())
                        {
                            auto idx2 = parent->getChildIndex(object);
                            if (idx2 > idx) {
                                parent->setChildIndex(object, idx);
                            }
                            else if (idx2 < idx) {
                                parent->setChildIndex(object, idx - 1);
                            }
                        }
                        else {
                            parent->setChildIndex(object, idx);
                        }
                    }
                }
            }
        });
        objDDTarget->getOnBottomDataReceivedEvent().addListener([objId](auto id) {
            if (Editor::getCurrentScene() == nullptr) return;
            auto currObject = Editor::getCurrentScene()->findObjectById(objId);
            auto parent = currObject->getParent();
            if (parent == nullptr) return; // Skip root nodes
            bool isPrefabScene = parent->getScene()->getObjects().size() > 0 && parent->getScene()->getObjects()[0]->isPrefab();
            if (!parent->isInPrefab() || isPrefabScene) {
                auto object = Editor::getCurrentScene()->findObjectById(id);
                if (!parent->isRelative(id)) {
                    if (object->isPrefab() || !object->isInPrefab() || isPrefabScene) {
                        auto idx = parent->getChildIndex(currObject);
                        if (parent == object->getParent())
                        {
                            auto idx2 = parent->getChildIndex(object);
                            if (idx2 > idx) {
                                parent->setChildIndex(object, idx + 1);
                            }
                            else if (idx2 < idx) {
                                parent->setChildIndex(object, idx);
                            }
                        }
                        else {
                            parent->setChildIndex(object, idx + 1);
                        }
                    }
                }
            }
        });

        auto prefabTarget = node->addPlugin<DDTargetTopBottomPlugin<std::string>>(GetFileExtensionSuported(E_FileExts::Prefab)[0]);
        prefabTarget->getOnDataReceivedEvent().addListener([objId](auto path) {
            auto parent = Editor::getCurrentScene()->findObjectById(objId);
            if (parent) {
                if (!parent->isInPrefab()) {
                    Editor::getCurrentScene()->loadPrefab(objId, path);
                }
            }
        });
        prefabTarget->getOnTopDataReceivedEvent().addListener([objId](auto path) {
            auto currObject = Editor::getCurrentScene()->findObjectById(objId);
            auto parent = currObject->getParent();
            if (parent == nullptr) return; // Skip root nodes
            if (parent) {
                if (!parent->isInPrefab()) {
                    auto prefabObj = Editor::getCurrentScene()->loadPrefab(parent->getId(), path);
                    auto idx = parent->getChildIndex(currObject);
                    parent->setChildIndex(prefabObj, idx);
                }
            }
        });
        prefabTarget->getOnBottomDataReceivedEvent().addListener([objId](auto path) {
            auto currObject = Editor::getCurrentScene()->findObjectById(objId);
            auto parent = currObject->getParent();
            if (parent == nullptr) return; // Skip root nodes
            if (parent) {
                if (!parent->isInPrefab()) {
                    auto prefabObj = Editor::getCurrentScene()->loadPrefab(parent->getId(), path);
                    auto idx = parent->getChildIndex(currObject);
                    parent->setChildIndex(prefabObj, idx + 1);
                }
            }
        });


        node->addPlugin<DDSourcePlugin<uint64_t>>(EDragDropID::OBJECT, sceneObject.getName(), objId);

        auto color = sceneObject.isInPrefab() ? (sceneObject.isActive() ? ImVec4(PREFAB_COLOR) : ImVec4(PREFAB_DISABLED_COLOR)) : (sceneObject.isActive() ? ImVec4(NORMAL_COLOR) : ImVec4(DISABLED_COLOR));
        node->setColor(color);

        auto ctxMenu = node->addPlugin<ContextMenu>(sceneObject.getName() + "_Context");
        addCreationContextMenu(sceneObject, ctxMenu);

        m_objectNodeMap[objId] = node;
    }

    void Hierarchy::onSceneObjectDeleted(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());
        if (nodePair != m_objectNodeMap.end())
        {
            if(nodePair->second->hasContainer())
                nodePair->second->getContainer()->removeWidget(nodePair->second);
            nodePair->second = nullptr;
            m_objectNodeMap.erase(nodePair);
        }
    }

    void Hierarchy::onSceneObjectAttached(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());

        if (nodePair != m_objectNodeMap.end())
        {
            auto widget = nodePair->second;
            if (widget->hasContainer())
                widget->getContainer()->removeWidget(widget);

            auto parent = sceneObject.getParent();
            if (parent != nullptr)
            {
                auto obj = sceneObject.getSharedPtr();
                auto idx = parent->getChildIndex(obj);
                auto parentWidget = m_objectNodeMap.at(parent->getId());
                parentWidget->setIsLeaf(false);
                parentWidget->addWidget(widget, idx);
                parentWidget->open();
            }
            auto color = sceneObject.isInPrefab() ? (sceneObject.isActive() ? ImVec4(PREFAB_COLOR) : ImVec4(PREFAB_DISABLED_COLOR)): (sceneObject.isActive() ? ImVec4(NORMAL_COLOR) : ImVec4(DISABLED_COLOR));            
            widget->setColor(color);
        }
    }

    void Hierarchy::onSceneObjectDetached(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());

        if (nodePair != m_objectNodeMap.end())
        {
            if (sceneObject.getParent() && sceneObject.getParent()->getChildren().size() == 0)
            {
                if (m_objectNodeMap.count(sceneObject.getParent()->getId()) > 0)
                {
                    auto parentWidget = m_objectNodeMap.at(sceneObject.getParent()->getId());
                    parentWidget->setIsLeaf(true);

                    auto widget = nodePair->second;
                    if (widget->hasContainer())
                        widget->getContainer()->removeWidget(widget);
                }
            }
        }
    }

    void Hierarchy::onSceneObjectSelected(SceneObject& sceneObject)
    {
        if (sceneObject.isSelected()) {            
            Editor::getInstance()->addTarget(sceneObject.getSharedPtr());
        }
    }

    void Hierarchy::onSceneObjectActivated(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());
        if (nodePair != m_objectNodeMap.end()) {
            auto color = sceneObject.isInPrefab() ? (sceneObject.isActive() ? ImVec4(PREFAB_COLOR) : ImVec4(PREFAB_DISABLED_COLOR)) : (sceneObject.isActive() ? ImVec4(NORMAL_COLOR) : ImVec4(DISABLED_COLOR));
            nodePair->second->setColor(color);
        }
    }

    void Hierarchy::onSceneObjectChangedName(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());
        if (nodePair != m_objectNodeMap.end())
            nodePair->second->setName(sceneObject.getName());
    }

    void Hierarchy::onTargetAdded(const std::shared_ptr<SceneObject>& object)
    {
        if (!object) return;

        // Update current selected id
        auto nodePair = m_objectNodeMap.find(object->getId());
        if (nodePair != m_objectNodeMap.end())
        {
            nodePair->second->setIsSelected(true);

            // Recursive open parent nodes
            auto parent = object->getParent();
            while (parent != nullptr)
            {
                auto parentWidget = m_objectNodeMap.at(parent->getId());
                if (parentWidget->isOpened())
                    break;
                parentWidget->open();
                parent = parent->getParent();
            }

            // Set this node open as well
            nodePair->second->open();
            object->getNameChangedEvent().addListener(std::bind(&Hierarchy::onSceneObjectChangedName, this, std::placeholders::_1));
        }

        // Initialize Inspector
        Editor::getCanvas()->getInspector()->initialize();
    }

    void Hierarchy::onTargetRemoved(const std::shared_ptr<SceneObject>& object)
    {
        if (!object) return;
        auto nodePair = m_objectNodeMap.find(object->getId());
        if (nodePair != m_objectNodeMap.end())
        {
            nodePair->second->setIsSelected(false);
            object->getNameChangedEvent().removeAllListeners();
        }

        // Initialize Inspector
        Editor::getCanvas()->getInspector()->initialize();
    }

    void Hierarchy::setNodeHighlight(uint64_t nodeId, bool highlight)
    {
        auto nodePair = m_objectNodeMap.find(nodeId);
        if (nodePair != m_objectNodeMap.end())
        {
            nodePair->second->setHighlighted(highlight);

            if (highlight)
            {
                m_highlightTimer.setTimeOut<uint64_t, bool>(1000, (std::function<void(uint64_t, bool)>)std::bind(&Hierarchy::setNodeHighlight, this, std::placeholders::_1, std::placeholders::_2), nodeId, false);
            }
        }
    }

    void Hierarchy::onTargetCleared()
    {
        if (Editor::getCurrentScene() == nullptr)
        {
            clear();
            return;
        }

        std::shared_ptr<SceneObject> object = nullptr;
        for (auto& pair : m_objectNodeMap)
        {
            pair.second->setIsSelected(false);
            object = Editor::getCurrentScene()->findObjectById(pair.first);
            if(object) object->getNameChangedEvent().removeAllListeners();
        }

        // Initialize Inspector
        Editor::getCanvas()->getInspector()->initialize();
    }

    bool Hierarchy::rebuildHierarchy()
    {
        auto scene = Editor::getCurrentScene();
        if (scene)
        {
            clear();
            initialize();
            for (const auto& obj : scene->getObjects()) {
                onSceneObjectCreated(*obj);
                if (obj->getParent())
                    onSceneObjectAttached(*obj);
            }
            return true;
        }
        return false;
    }

    void Hierarchy::addCreationContextMenu(SceneObject& sceneObject, std::shared_ptr<ContextMenu> ctxMenu)
    {
        if (ctxMenu == nullptr) return;

        if (sceneObject.isInPrefab())
        {
            auto prefabId = sceneObject.getPrefabRootId();
            auto objectId = sceneObject.getId();
            ctxMenu->createWidget<MenuItem>("Open Prefab")->getOnClickEvent().addListener([prefabId](auto widget) {
                TaskManager::getInstance()->addTask([prefabId]() {
                    Editor::getInstance()->openPrefabById(prefabId);
                });
            });
            ctxMenu->createWidget<MenuItem>("Reload Prefab")->getOnClickEvent().addListener([objectId](auto widget) {
                TaskManager::getInstance()->addTask([objectId]() {
                    Editor::getInstance()->reloadPrefab(objectId);
                });
            });
            ctxMenu->createWidget<MenuItem>("Unpack Prefab")->getOnClickEvent().addListener([objectId](auto widget) {
                TaskManager::getInstance()->addTask([objectId]() {
                    Editor::getInstance()->unpackPrefab(objectId);
                });
            });
        }

        auto createMenu = ctxMenu->createWidget<Menu>("Create");
        createMenu->createWidget<MenuItem>("New Object")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([&]() {
                auto target = Editor::getInstance()->getFirstTarget();
                auto newObject = Editor::getCurrentScene()->createObject("New Object", target);
                CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
            });
        });

        // Camera
        createMenu->createWidget<MenuItem>("Camera")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([&]() {
                auto target = Editor::getInstance()->getFirstTarget();
                auto newObject = Editor::getCurrentScene()->createObject("Camera", target);
                newObject->addComponent<CameraComponent>("camera");
                newObject->addComponent<FigureComponent>(GetEnginePath("figures/camera"))->setSkipSerialize(true);
                CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
            });
        });

        // Primitives
        {
            auto shapeMenu = createMenu->createWidget<Menu>("Primitive");
            shapeMenu->createWidget<MenuItem>("Cube")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Cube", target);
                    newObject->addComponent<FigureComponent>("figures/cube.dae");
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            shapeMenu->createWidget<MenuItem>("Plane")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Plane", target);
                    newObject->addComponent<FigureComponent>("figures/plane.dae");
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            shapeMenu->createWidget<MenuItem>("Sphere")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Sphere", target);
                    newObject->addComponent<FigureComponent>("figures/sphere.dae");
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            shapeMenu->createWidget<MenuItem>("Cone")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Cone", target);
                    newObject->addComponent<FigureComponent>("figures/cone.dae");
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            shapeMenu->createWidget<MenuItem>("Cylinder")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Cylinder", target);
                    newObject->addComponent<FigureComponent>("figures/cylinder.dae");
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            shapeMenu->createWidget<MenuItem>("Torus")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Torus", target);
                    newObject->addComponent<FigureComponent>("figures/torus.dae");
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });
        }

        // Lights
        {
            auto lightMenu = createMenu->createWidget<Menu>("Light");
            lightMenu->createWidget<MenuItem>("Directional Light")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    if (!Editor::getCurrentScene()->isDirectionalLightAvailable())
                    {
                        auto msgBox = MsgBox("Error", "Max number of Directional Light reached!", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::error);
                        while (!msgBox.ready(1000));
                        return;
                    }
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Directional Light", target);
                    newObject->addComponent<DirectionalLight>();
                    newObject->getTransform()->setLocalPosition({ 0.f, 5.f, 0.f });
                    newObject->getTransform()->setLocalRotation({ DEGREES_TO_RADIANS(90.f), 0.f, .0f });
                    newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/direct-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            lightMenu->createWidget<MenuItem>("Point Light")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    if (!Editor::getCurrentScene()->isPointLightAvailable())
                    {
                        auto msgBox = MsgBox("Error", "Max number of Point Light reached!", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::error);
                        while (!msgBox.ready(1000));
                        return;
                    }
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Point Light", target);
                    newObject->addComponent<PointLight>();
                    newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/point-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            lightMenu->createWidget<MenuItem>("Spot Light")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    if (!Editor::getCurrentScene()->isSpotLightAvailable())
                    {
                        auto msgBox = MsgBox("Error", "Max number of Spot Light reached!", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::error);
                        while (!msgBox.ready(1000));
                        return;
                    }
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Spot Light", target);
                    newObject->addComponent<SpotLight>();
                    newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/spot-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });
        }

        // Audio
        {
            auto audioMenu = createMenu->createWidget<Menu>("Audio");
            audioMenu->createWidget<MenuItem>("Audio Source")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Audio Source", target);
                    newObject->addComponent<AudioSource>();
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            audioMenu->createWidget<MenuItem>("Audio Listener")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Audio Listener", target);
                    newObject->addComponent<AudioListener>();
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });
        }

        // Text
        {
            auto textMenu = createMenu->createWidget<Menu>("Text");
            textMenu->createWidget<MenuItem>("Text")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Text", target);
                    newObject->addComponent<TextComponent>();
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                    });
                });
        }

        // Effects
        {
            auto effectMenu = createMenu->createWidget<Menu>("Effect");
            effectMenu->createWidget<MenuItem>("Particle")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Particle", target);
                    newObject->addComponent<Particle>();
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });
        }

        // GUI
        {
            auto guiMenu = createMenu->createWidget<Menu>("GUI");
            guiMenu->createWidget<MenuItem>("UIImage")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UIImage", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                    if (rect) {
                        newObject->addComponent<UIImage>("sprites/background", rect->getSize());
                    }
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            guiMenu->createWidget<MenuItem>("UIText")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UIText", target, true);
                    newObject->addComponent<UIText>("Text");
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            guiMenu->createWidget<MenuItem>("UITextField")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UITextField", target, true);
                    newObject->addComponent<UITextField>("TextField");
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            guiMenu->createWidget<MenuItem>("UIButton")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UIButton", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                    newObject->addComponent<UIButton>("sprites/background", rect->getSize());
                    auto labelObject = Editor::getCurrentScene()->createObject("Label", newObject, true, Vec2());
                    labelObject->addComponent<UIText>("Button");
                    auto pos = labelObject->getRectTransform()->getLocalPosition();
                    labelObject->getRectTransform()->setLocalPosition({ pos[0], pos[1], 0.01f });
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            guiMenu->createWidget<MenuItem>("UISlider")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UISlider", target, true, Vec2(160.f, 40.f));
                    // Create Slider
                    auto sliderComp = newObject->addComponent<UISlider>();
                    // Create Background
                    auto newBG = Editor::getCurrentScene()->createObject("background", newObject, true, Vec2(160.f, 16.f));
                    auto rectBG = std::dynamic_pointer_cast<RectTransform>(newBG->getTransform());
                    auto imgBG = newBG->addComponent<UIImage>("sprites/background", rectBG->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    imgBG->setColor(0.84f, 0.84f, 0.84f, 1);
                    newBG->setIsRaycastTarget(false);
                    if (rectBG) {
                        rectBG->setAnchor(Vec4(0.f, 0.f, 1.f, 1.f));
                        rectBG->setAnchoredPosition(Vec2(0, 0));
                        auto pos = rectBG->getLocalPosition();
                        rectBG->setLocalPosition({ pos[0], pos[1], 0.01f });
                    }
                    // Create Fill
                    auto newFillArea = Editor::getCurrentScene()->createObject("fillArea", newObject, true, Vec2(158.f, 14.f));
                    newFillArea->setIsRaycastTarget(false);
                    auto rectFillArea = std::dynamic_pointer_cast<RectTransform>(newFillArea->getTransform());
                    if (rectFillArea) {
                        rectFillArea->setAnchor(Vec4(0.f, 0.f, 1.f, 1.f));
                        auto pos = rectFillArea->getLocalPosition();
                        rectFillArea->setLocalPosition({ pos[0], pos[1], 0.02f });
                    }

                    auto newFill = Editor::getCurrentScene()->createObject("fill", newFillArea, true, Vec2(1.f, 14.f));
                    auto rectFill = std::dynamic_pointer_cast<RectTransform>(newFill->getTransform());
                    if (rectFill) {
                        rectFill->setAnchor(Vec4(0.f, 0.f, 0.f, 1.f));
                        rectFill->setAnchoredPosition(Vec2(0, 0));
                        auto pos = rectFill->getLocalPosition();
                        rectFill->setLocalPosition({ pos[0], pos[1], 0.03f });
                    }
                    auto imgFill = newFill->addComponent<UIImage>("sprites/background", rectFill->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    newFill->setIsRaycastTarget(false);
                    // Create Handle
                    auto newHandleArea = Editor::getCurrentScene()->createObject("handleArea", newObject, true, Vec2(140.f, 30.f));
                    newHandleArea->setIsRaycastTarget(false);
                    auto rectHandleArea = std::dynamic_pointer_cast<RectTransform>(newHandleArea->getTransform());
                    if (rectHandleArea) {
                        rectHandleArea->setAnchor(Vec4(0.f, 0.f, 1.f, 1.f));
                        rectHandleArea->setAnchoredPosition(Vec2(0, 0));
                        auto pos = rectHandleArea->getLocalPosition();
                        rectHandleArea->setLocalPosition({ pos[0], pos[1], 0.02f });
                    }
                    auto newHandle = Editor::getCurrentScene()->createObject("handle", newHandleArea, true, Vec2(30.f, 30.f));
                    auto rectHandle = std::dynamic_pointer_cast<RectTransform>(newHandle->getTransform());
                    if (rectHandle) {
                        rectHandle->setAnchor(Vec4(0.f, 0.f, 0.f, 1.f));
                        rectHandle->setAnchoredPosition(Vec2(0, 0));
                        auto pos = rectHandle->getLocalPosition();
                        rectHandle->setLocalPosition({ pos[0], pos[1], 0.03f });
                    }
                    newHandle->addComponent<UIImage>("sprites/background", rectHandle->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    newHandle->setIsRaycastTarget(false);
                    sliderComp->setFillObject(newFill);
                    sliderComp->setHandleObject(newHandle);
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            guiMenu->createWidget<MenuItem>("UIScrollView")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UIScrollView", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                    rect->setSize(Vec2(200, 200));
                    auto pos = rect->getLocalPosition();
                    rect->setLocalPosition({ pos[0], pos[1], 0.01f});

                    // Create ScrollView
                    auto uiScrollView = newObject->addComponent<UIScrollView>("sprites/background", rect->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    if (uiScrollView) {
                        uiScrollView->setAlpha(0.4f);
                    }

                    // Create Horizontal ScrollBar
                    auto newScrollBar  = Editor::getCurrentScene()->createObject("Scrollbar Horizontal", newObject, true, rect->getSize());
                    auto rectScrollBar  = std::dynamic_pointer_cast<RectTransform>(newScrollBar ->getTransform());
                    if (rectScrollBar )
                    {
                        rectScrollBar ->setAnchor(Vec4(0, 0, 1, 0));
                        auto offsetH = rectScrollBar ->getOffset();
                        offsetH[0] = 0;
                        offsetH[2] = 17;
                        rectScrollBar ->setOffset(offsetH);
                        auto sizeH = rectScrollBar ->getSize();
                        sizeH[1] = 20;
                        rectScrollBar ->setSize(sizeH);
                        rectScrollBar ->setPivot(Vec2(0, 0));
                        auto anchoredPosH = rectScrollBar ->getAnchoredPosition();
                        anchoredPosH[1] = 0;
                        rectScrollBar ->setAnchoredPosition(anchoredPosH);
                        auto pos = rectScrollBar ->getLocalPosition();
                        rectScrollBar ->setLocalPosition({ pos[0], pos[1], 0.01f });                        
                    }
                    auto uiScrollBar  = newScrollBar ->addComponent<UIScrollBar>("sprites/background", rectScrollBar ->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    if (uiScrollBar )
                    {
                        uiScrollBar ->setDirection(UIScrollBar::Direction::LeftToRight, false);
                        uiScrollBar ->setColor(Vec4(0.8392158f, 0.8392158f, 0.8392158f, 1.f));
                    }

                    // Create Horizontal Sliding Area
                    auto newHorizontalSliding = Editor::getCurrentScene()->createObject("Sliding Area", newScrollBar , true, rectScrollBar ->getSize());
                    auto rectHorizontalSliding = std::dynamic_pointer_cast<RectTransform>(newHorizontalSliding->getTransform());
                    rectHorizontalSliding->setAnchor(Vec4(0, 0, 1, 1));
                    rectHorizontalSliding->setOffset(Vec4(10, 10, 10, 10));
                    pos = rectHorizontalSliding->getLocalPosition();
                    rectHorizontalSliding->setLocalPosition({ pos[0], pos[1], 0.02f });

                    // Create Horizontal Handle
                    auto newHorizontalHandle = Editor::getCurrentScene()->createObject("Handle", newHorizontalSliding, true, rectHorizontalSliding->getSize());
                    auto rectHorizontalHandle = std::dynamic_pointer_cast<RectTransform>(newHorizontalHandle->getTransform());
                    if (rectHorizontalHandle)
                    {
                        rectHorizontalHandle->setAnchor(Vec4(0, 0, 1, 1));
                        rectHorizontalHandle->setOffset(Vec4(-10, -10, -10, -10));
                        pos = rectHorizontalHandle->getLocalPosition();
                        rectHorizontalHandle->setLocalPosition({ pos[0], pos[1], 0.03f });
                    }
                    auto horizontalImg = newHorizontalHandle->addComponent<UIImage>("sprites/background", rectHorizontalHandle->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    uiScrollBar ->setHandle(newHorizontalHandle);

                    // Create Vertical ScrollBar
                    auto newVerticalBar = Editor::getCurrentScene()->createObject("Scrollbar Vertical", newObject, true, rect->getSize());
                    auto rectVerticalBar = std::dynamic_pointer_cast<RectTransform>(newVerticalBar->getTransform());
                    if (rectVerticalBar)
                    {
                        rectVerticalBar->setAnchor(Vec4(1, 0, 1, 1));
                        auto offsetV = rectVerticalBar->getOffset();
                        offsetV[1] = 17;
                        offsetV[3] = 0;
                        rectVerticalBar->setOffset(offsetV);
                        auto sizeV = rectVerticalBar->getSize();
                        sizeV[0] = 20;
                        rectVerticalBar->setSize(sizeV);
                        rectVerticalBar->setPivot(Vec2(1, 1));
                        auto anchoredPosV = rectVerticalBar->getAnchoredPosition();
                        anchoredPosV[0] = 0;
                        rectVerticalBar->setAnchoredPosition(anchoredPosV);
                        pos = rectVerticalBar->getLocalPosition();
                        rectVerticalBar->setLocalPosition({ pos[0], pos[1], 0.01f });
                    }
                    auto uiVerticalBar = newVerticalBar->addComponent<UIScrollBar>("sprites/background", rectVerticalBar->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    if (uiVerticalBar)
                    {
                        uiVerticalBar->setDirection(UIScrollBar::Direction::BottomToTop, false);
                        uiVerticalBar->setColor(Vec4(0.8392158f, 0.8392158f, 0.8392158f, 1.f));
                    }

                    // Create Vertical Sliding Area
                    auto newVerticalSliding = Editor::getCurrentScene()->createObject("Sliding Area", newVerticalBar, true, rectVerticalBar->getSize());
                    auto rectVerticalSliding = std::dynamic_pointer_cast<RectTransform>(newVerticalSliding->getTransform());
                    rectVerticalSliding->setAnchor(Vec4(0, 0, 1, 1));
                    rectVerticalSliding->setOffset(Vec4(10, 10, 10, 10));
                    pos = rectVerticalSliding->getLocalPosition();
                    rectVerticalSliding->setLocalPosition({ pos[0], pos[1], 0.02f });

                    // Create Vertical Handle
                    auto newVerticalHandle = Editor::getCurrentScene()->createObject("Handle", newVerticalSliding, true, rectVerticalSliding->getSize());
                    auto rectVerticalHandle = std::dynamic_pointer_cast<RectTransform>(newVerticalHandle->getTransform());
                    if (rectVerticalHandle)
                    {
                        rectVerticalHandle->setAnchor(Vec4(0, 0, 1, 1));
                        rectVerticalHandle->setOffset(Vec4(-10, -10, -10, -10));
                        pos = rectVerticalHandle->getLocalPosition();
                        rectVerticalHandle->setLocalPosition({ pos[0], pos[1], 0.03f });
                    }
                    auto verticalImg = newVerticalHandle->addComponent<UIImage>("sprites/background", rectVerticalHandle->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));

                    uiVerticalBar->setHandle(newVerticalHandle);

                    //!!! Mask must be created last => to prevent bug from stencil render
                    // Create Background
                    auto newMask = Editor::getCurrentScene()->createObject("UIMask", newObject, true, rect->getSize());
                    auto rectMask = std::dynamic_pointer_cast<RectTransform>(newMask->getTransform());
                    if (rectMask)
                    {
                        rectMask->setAnchor(Vec4(0, 0, 1, 1));
                        rectMask->setOffset(Vec4(0, 20, 20, 0));
                        pos = rectMask->getLocalPosition();
                        rectMask->setLocalPosition({ pos[0], pos[1], 0.01f });
                    }

                    // Create Content Mask
                    auto uiMask = newMask->addComponent<UIMask>("", rectMask->getSize());

                    // Create Content
                    auto newContent = Editor::getCurrentScene()->createObject("Content", newMask, true, rectMask->getSize());
                    auto rectContent = std::dynamic_pointer_cast<RectTransform>(newContent->getTransform());
                    if (rectContent)
                    {
                        rectContent->setAnchor(Vec4(0.f, 1.f, 0.f, 1.f));
                        rectContent->setPivot(Vec2(0, 1));
                        rectContent->setPosition({ 0, 0, 0});
                        rectContent->setSize({300, 300});
                        pos = rectContent->getLocalPosition();
                        rectContent->setLocalPosition({ pos[0], pos[1], 0.02f });
                    }

                    uiScrollView->setContent(newContent);
                    uiScrollView->setViewport(newMask);
                    uiScrollView->setHorizontalScrollBar(uiScrollBar );
                    uiScrollView->setVerticalScrollBar(uiVerticalBar);

                    uiScrollBar ->setValue(1.f);
                    uiVerticalBar->setValue(0.f);

                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });

            guiMenu->createWidget<MenuItem>("UIMask")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UIMask", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());

                    // Create Mask
                    auto uiMask = newObject->addComponent<UIMask>("", rect->getSize());
                    CommandManager::getInstance()->PushCommand(ige::creator::COMMAND_TYPE::ADD_OBJECT, newObject);
                });
            });
        }
        ctxMenu->createWidget<MenuItem>("Copy")->getOnClickEvent().addListener([this](auto widget) {
            m_bSkipDeselect = true;
            TaskManager::getInstance()->addTask([&]() {
                Editor::getInstance()->copyObject();
                m_bSkipDeselect = false;
            });
        });
        ctxMenu->createWidget<MenuItem>("Paste")->getOnClickEvent().addListener([this](auto widget) {
            m_bSkipDeselect = true;
            TaskManager::getInstance()->addTask([&]() {
                Editor::getInstance()->pasteObject();
                m_bSkipDeselect = false;
            });
        });
        ctxMenu->createWidget<MenuItem>("Duplicate")->getOnClickEvent().addListener([this](auto widget) {
            m_bSkipDeselect = true;
            TaskManager::getInstance()->addTask([&]() {
                Editor::getInstance()->cloneObject();
                m_bSkipDeselect = false;
            });
        });
        ctxMenu->createWidget<MenuItem>("Delete")->getOnClickEvent().addListener([this](auto widget) {
            m_bSkipDeselect = true;
            TaskManager::getInstance()->addTask([&]() {
                Editor::getInstance()->deleteObject();
                m_bSkipDeselect = false;
            });
        });
    }

    void Hierarchy::initialize()
    {
        if (!m_bInitialized)
        {
            auto scene = SceneManager::getInstance()->getCurrentScene();
            auto isPrefab = scene && scene->isPrefab();
            m_groupLayout = createWidget<Group>("Prefab", isPrefab, isPrefab);
            m_groupLayout->getOnClosedEvent().addListener([]() {
                TaskManager::getInstance()->addTask([] {
                    Editor::getInstance()->closePrefab();
                });
            });
            m_bInitialized = true;
        }
    }

    void Hierarchy::drawWidgets()
    {
        Panel::drawWidgets();
    }

    void Hierarchy::clear()
    {
        for (auto& [key, value] : m_objectNodeMap) {
            value = nullptr;
        }
        m_objectNodeMap.clear();
        m_groupLayout = nullptr;
        removeAllWidgets();
        m_bInitialized = false;
    }
}
