#include <imgui.h>
#include <imgui_internal.h>

#include "core/filesystem/FileSystem.h"
#include "core/FileHandle.h"
#include "core/dialog/MsgBox.h"
#include "core/panels/Hierarchy.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/menu/ContextMenu.h"
#include "core/menu/MenuItem.h"
#include "core/task/TaskManager.h"
#include "core/plugin/DragDropPlugin.h"
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
#include "components/gui/UITextBitmap.h"
#include "components/gui/UITextField.h"
#include "components/gui/UIButton.h"
#include "components/gui/UISlider.h"
#include "components/gui/UIScrollView.h"
#include "components/gui/UIScrollBar.h"
#include "components/gui/UIMask.h"
#include "components/audio/AudioSource.h"
#include "components/audio/AudioListener.h"
#include "components/particle/Particle.h"

#include <scene/Scene.h>
#include <scene/SceneObject.h>
using namespace ige::scene;

#include <utils/PyxieHeaders.h>

#define NORMAL_COLOR {1.f, 1.f, 1.f, 1.f}
#define PREFAB_COLOR {0.f, 0.85f, 0.85f, 1.f}

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

        node->addPlugin<DDTargetPlugin<uint64_t>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([objId](auto txt) {
            auto parent = Editor::getCurrentScene()->findObjectById(objId);
            auto object = Editor::getCurrentScene()->findObjectById(txt);
            bool isPrefabScene = parent->getScene()->getObjects().size() > 0 && parent->getScene()->getObjects()[0]->isPrefab();
            if (!parent->isInPrefab() || isPrefabScene)
            {
                auto loop = false;
                for (auto& target : Editor::getInstance()->getTarget()->getAllTargets())
                {
                    if (!target.expired()
                        && parent->isRelative(target.lock()->getId()))
                    {
                        loop = true; break;
                    }
                }
                if (!loop)
                {
                    std::vector<std::shared_ptr<SceneObject>> movingObjects = {};
                    for (auto& target : Editor::getInstance()->getTarget()->getAllTargets())
                    {
                        if (!target.expired() && (!target.lock()->getParent() || target.lock()->getParent()->isSelected() == false))
                            movingObjects.push_back(target.lock());
                    }
                    for (auto& target : movingObjects)
                    {
                        if(target->isPrefab() || !target->isInPrefab() || isPrefabScene)
                            target->setParent(parent);
                    }
                }
            }            
        });
        node->addPlugin<DDTargetPlugin<std::string>>(GetFileExtensionSuported(E_FileExts::Prefab)[0])->getOnDataReceivedEvent().addListener([objId](auto path) {
            auto parent = Editor::getCurrentScene()->findObjectById(objId);
            if (parent) {
                if (!parent->isInPrefab()) {
                    Editor::getCurrentScene()->loadPrefab(objId, path);
                }
            }
        });

        node->addPlugin<DDSourcePlugin<uint64_t>>(EDragDropID::OBJECT, sceneObject.getName(), objId);

        auto color = sceneObject.isInPrefab() ? ImVec4(PREFAB_COLOR) : ImVec4(NORMAL_COLOR);
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
                auto parentWidget = m_objectNodeMap.at(parent->getId());
                parentWidget->setIsLeaf(false);
                parentWidget->addWidget(widget);
                parentWidget->open();
            }
            auto color = sceneObject.isInPrefab() ? ImVec4(PREFAB_COLOR) : ImVec4(NORMAL_COLOR);
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
                if (obj->getParent()) onSceneObjectAttached(*obj);
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
            auto prefabId = sceneObject.getPrefabIdRecursive();
            ctxMenu->createWidget<MenuItem>("Open Prefab")->getOnClickEvent().addListener([prefabId](auto widget) {
                TaskManager::getInstance()->addTask([prefabId]() {
                    Editor::getInstance()->openPrefabById(prefabId);
                });
            });
        }

        auto createMenu = ctxMenu->createWidget<Menu>("Create");
        createMenu->createWidget<MenuItem>("New Object")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([&]() {
                auto target = Editor::getInstance()->getFirstTarget();
                auto newObject = Editor::getCurrentScene()->createObject("New Object", target);
                Editor::getInstance()->addTarget(newObject, true);
            });
        });

        // Camera
        createMenu->createWidget<MenuItem>("Camera")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([&]() {
                auto target = Editor::getInstance()->getFirstTarget();
                auto newObject = Editor::getCurrentScene()->createObject("Camera", target);
                newObject->addComponent<CameraComponent>("camera");
                newObject->addComponent<FigureComponent>(GetEnginePath("figures/camera"))->setSkipSerialize(true);
                Editor::getInstance()->addTarget(newObject, true);
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
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            shapeMenu->createWidget<MenuItem>("Plane")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Plane", target);
                    newObject->addComponent<FigureComponent>("figures/plane.dae");
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            shapeMenu->createWidget<MenuItem>("Sphere")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Sphere", target);
                    newObject->addComponent<FigureComponent>("figures/sphere.dae");
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            shapeMenu->createWidget<MenuItem>("Cone")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Cone", target);
                    newObject->addComponent<FigureComponent>("figures/cone.dae");
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            shapeMenu->createWidget<MenuItem>("Cylinder")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Cylinder", target);
                    newObject->addComponent<FigureComponent>("figures/cylinder.dae");
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            shapeMenu->createWidget<MenuItem>("Torus")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Torus", target);
                    newObject->addComponent<FigureComponent>("figures/torus.dae");
                    Editor::getInstance()->addTarget(newObject, true);
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
                    newObject->getTransform()->setPosition({ 0.f, 5.f, 0.f });
                    newObject->getTransform()->setRotation({ DEGREES_TO_RADIANS(90.f), 0.f, .0f });
                    newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/direct-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                    Editor::getInstance()->addTarget(newObject, true);
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
                    Editor::getInstance()->addTarget(newObject, true);
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
                    Editor::getInstance()->addTarget(newObject, true);
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
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            audioMenu->createWidget<MenuItem>("Audio Listener")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("Audio Listener", target);
                    newObject->addComponent<AudioListener>();
                    Editor::getInstance()->addTarget(newObject, true);
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
                    Editor::getInstance()->addTarget(newObject, true);
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
                    newObject->addComponent<UIImage>("sprites/background", rect->getSize());
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            guiMenu->createWidget<MenuItem>("UIText")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UIText", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                    newObject->addComponent<UIText>("Text");
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            guiMenu->createWidget<MenuItem>("UITextBitmap")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                auto target = Editor::getInstance()->getFirstTarget();
                auto newObject = Editor::getCurrentScene()->createObject("UITextBitmap", target, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UITextBitmap>("Text");
                Editor::getInstance()->addTarget(newObject, true);
                });
            });

            guiMenu->createWidget<MenuItem>("UITextField")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UITextField", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                    newObject->addComponent<UIImage>("sprites/background", rect->getSize());
                    auto newLabel = Editor::getCurrentScene()->createObject("Label", newObject, true, Vec2());
                    newLabel->addComponent<UITextField>("TextField");
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            guiMenu->createWidget<MenuItem>("UIButton")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newObject = Editor::getCurrentScene()->createObject("UIButton", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                    newObject->addComponent<UIButton>("sprites/background", rect->getSize());
                    Editor::getCurrentScene()->createObject("Label", newObject, true, Vec2())->addComponent<UIText>("Button");
                    Editor::getInstance()->addTarget(newObject, true);
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
                    }
                    // Create Fill
                    auto newFillArea = Editor::getCurrentScene()->createObject("fillArea", newObject, true, Vec2(158.f, 14.f));
                    newFillArea->setIsRaycastTarget(false);
                    auto rectFillArea = std::dynamic_pointer_cast<RectTransform>(newFillArea->getTransform());
                    if (rectFillArea) {
                        rectFillArea->setAnchor(Vec4(0.f, 0.f, 1.f, 1.f));
                    }

                    auto newFill = Editor::getCurrentScene()->createObject("fill", newFillArea, true, Vec2(1.f, 14.f));
                    auto rectFill = std::dynamic_pointer_cast<RectTransform>(newFill->getTransform());
                    if (rectFill) {
                        rectFill->setAnchor(Vec4(0.f, 0.f, 0.f, 1.f));
                        rectFill->setAnchoredPosition(Vec2(0, 0));
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
                    }
                    auto newHandle = Editor::getCurrentScene()->createObject("handle", newHandleArea, true, Vec2(30.f, 30.f));

                    auto rectHandle = std::dynamic_pointer_cast<RectTransform>(newHandle->getTransform());
                    if (rectHandle) {
                        rectHandle->setAnchor(Vec4(0.f, 0.f, 0.f, 1.f));
                        rectHandle->setAnchoredPosition(Vec2(0, 0));
                    }
                    newHandle->addComponent<UIImage>("sprites/background", rectHandle->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    newHandle->setIsRaycastTarget(false);
                    sliderComp->setFillObject(newFill);
                    sliderComp->setHandleObject(newHandle);
                    Editor::getInstance()->addTarget(newObject, true);
                });
            });

            guiMenu->createWidget<MenuItem>("UIScrollView")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newScrollView = Editor::getCurrentScene()->createObject("UIScrollView", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newScrollView->getTransform());
                    rect->setSize(Vec2(200, 200));

                    // Create ScrollView
                    auto uiScrollView = newScrollView->addComponent<UIScrollView>("sprites/background", rect->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    if (uiScrollView) {
                        uiScrollView->setAlpha(0.4f);
                    }

                    // Create Horizontal ScrollBar
                    auto newHorizontalBar = Editor::getCurrentScene()->createObject("Scrollbar Horizontal", newScrollView, true, rect->getSize());
                    auto rectHorizontalBar = std::dynamic_pointer_cast<RectTransform>(newHorizontalBar->getTransform());
                    if (rectHorizontalBar)
                    {
                        rectHorizontalBar->setAnchor(Vec4(0, 0, 1, 0));
                        auto offsetH = rectHorizontalBar->getOffset();
                        offsetH[0] = 0;
                        offsetH[2] = 17;
                        rectHorizontalBar->setOffset(offsetH);
                        auto sizeH = rectHorizontalBar->getSize();
                        sizeH[1] = 20;
                        rectHorizontalBar->setSize(sizeH);
                        rectHorizontalBar->setPivot(Vec2(0, 0));
                        auto anchoredPosH = rectHorizontalBar->getAnchoredPosition();
                        anchoredPosH[1] = 0;
                        rectHorizontalBar->setAnchoredPosition(anchoredPosH);
                    }
                    auto uiHorizontalBar = newHorizontalBar->addComponent<UIScrollBar>("sprites/background", rectHorizontalBar->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    if (uiHorizontalBar)
                    {
                        uiHorizontalBar->setDirection(UIScrollBar::Direction::LeftToRight, false);
                        uiHorizontalBar->setColor(Vec4(0.8392158f, 0.8392158f, 0.8392158f, 1.f));
                    }

                    // Create Horizontal Sliding Area
                    auto newHorizontalSliding = Editor::getCurrentScene()->createObject("Sliding Area", newHorizontalBar, true, rectHorizontalBar->getSize());
                    auto rectHorizontalSliding = std::dynamic_pointer_cast<RectTransform>(newHorizontalSliding->getTransform());
                    rectHorizontalSliding->setAnchor(Vec4(0, 0, 1, 1));
                    rectHorizontalSliding->setOffset(Vec4(10, 10, 10, 10));

                    // Create Horizontal Handle
                    auto newHorizontalHandle = Editor::getCurrentScene()->createObject("Handle", newHorizontalSliding, true, rectHorizontalSliding->getSize());
                    auto rectHorizontalHandle = std::dynamic_pointer_cast<RectTransform>(newHorizontalHandle->getTransform());
                    if (rectHorizontalHandle)
                    {
                        rectHorizontalHandle->setAnchor(Vec4(0, 0, 1, 1));
                        rectHorizontalHandle->setOffset(Vec4(-10, -10, -10, -10));
                    }
                    auto horizontalImg = newHorizontalHandle->addComponent<UIImage>("sprites/background", rectHorizontalHandle->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                    uiHorizontalBar->setHandle(newHorizontalHandle);

                    // Create Vertical ScrollBar
                    auto newVerticalBar = Editor::getCurrentScene()->createObject("Scrollbar Vertical", newScrollView, true, rect->getSize());
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

                    // Create Vertical Handle
                    auto newVerticalHandle = Editor::getCurrentScene()->createObject("Handle", newVerticalSliding, true, rectVerticalSliding->getSize());
                    auto rectVerticalHandle = std::dynamic_pointer_cast<RectTransform>(newVerticalHandle->getTransform());
                    if (rectVerticalHandle)
                    {
                        rectVerticalHandle->setAnchor(Vec4(0, 0, 1, 1));
                        rectVerticalHandle->setOffset(Vec4(-10, -10, -10, -10));
                        //rectVerticalHandle->setPivot(Vec2(1, 1));
                    }
                    auto verticalImg = newVerticalHandle->addComponent<UIImage>("sprites/background", rectVerticalHandle->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));

                    uiVerticalBar->setHandle(newVerticalHandle);

                    //!!! Mask must be created last => to prevent bug from stencil render
                    // Create Background
                    auto newMask = Editor::getCurrentScene()->createObject("UIMask", newScrollView, true, rect->getSize());
                    auto rectMask = std::dynamic_pointer_cast<RectTransform>(newMask->getTransform());
                    if (rectMask)
                    {
                        rectMask->setAnchor(Vec4(0, 0, 1, 1));
                        rectMask->setOffset(Vec4(0, 20, 20, 0));
                    }
                    auto uiMask = newMask->addComponent<UIMask>("sprites/background", rectMask->getSize());
                    uiMask->setAlpha(0);

                    // // Create Content
                    auto newContent = Editor::getCurrentScene()->createObject("Content", newMask, true, rectMask->getSize());
                    auto rectContent = std::dynamic_pointer_cast<RectTransform>(newContent->getTransform());
                    if (rectContent)
                    {
                        rectContent->setAnchor(Vec4(0, 1, 1, 1));
                        rectContent->setPivot(Vec2(0, 1));
                        auto size = rectContent->getSize();
                        size[1] = 300;
                        rectContent->setSize(size);
                    }

                    uiScrollView->setContent(newContent);
                    uiScrollView->setViewport(newMask);
                    uiScrollView->setHorizontalScrollBar(uiHorizontalBar);
                    uiScrollView->setVerticalScrollBar(uiVerticalBar);

                    uiVerticalBar->setValue(0.0f);

                    Editor::getInstance()->addTarget(newScrollView, true);
                });
            });

            guiMenu->createWidget<MenuItem>("UIMask")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([&]() {
                    auto target = Editor::getInstance()->getFirstTarget();
                    auto newMask = Editor::getCurrentScene()->createObject("UIMask", target, true);
                    auto rect = std::dynamic_pointer_cast<RectTransform>(newMask->getTransform());

                    // Create Mask
                    auto uiMask = newMask->addComponent<UIMask>("sprites/background", rect->getSize());
                    uiMask->setAlpha(0);

                    Editor::getInstance()->addTarget(newMask, true);
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
        // Draw widgets
        Panel::drawWidgets();

        // TODO: fix position calculation in some cases
        //if (m_bIsOpened && !m_bSkipDeselect && ImGui::IsWindowFocused() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        //    auto vMin = ImGui::GetWindowContentRegionMin();
        //    auto vMax = ImGui::GetWindowContentRegionMax();
        //    
        //    vMin.y += 6; // padding

        //    vMin.x += ImGui::GetWindowPos().x;
        //    vMin.y += ImGui::GetWindowPos().y;
        //    vMax.x += ImGui::GetWindowPos().x;
        //    vMax.y += ImGui::GetWindowPos().y;

        //    float nodeMapHeight = 0.f;
        //    for (const auto& item : m_objectNodeMap) {
        //        if (item.second && item.second->isOpened())
        //            nodeMapHeight += k_nodeDefaultHeight;
        //    }
        //    vMin.y += nodeMapHeight;

        //    if (ImGui::GetMousePos().x >= vMin.x && ImGui::GetMousePos().x <= vMax.x
        //        && ImGui::GetMousePos().y >= vMin.y && ImGui::GetMousePos().y <= vMax.y) {
        //        if(Editor::getCurrentScene())
        //            Editor::getInstance()->clearTargets();
        //    }
        //}
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
