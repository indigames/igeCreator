#include <imgui.h>
#include <imgui_internal.h>

#include "core/filesystem/FileSystem.h"
#include "core/FileHandle.h"
#include "core/dialog/MsgBox.h"
#include <scene/Scene.h>
#include <scene/SceneObject.h>
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
#include "components/gui/UITextField.h"
#include "components/gui/UIButton.h"
#include "components/gui/UISlider.h"
#include "components/audio/AudioSource.h"
#include "components/audio/AudioListener.h"
#include "components/particle/Particle.h"
using namespace ige::scene;

#include <utils/PyxieHeaders.h>

namespace ige::creator
{
    Hierarchy::Hierarchy(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        SceneObject::getCreatedEvent().addListener(std::bind(&Hierarchy::onSceneObjectCreated, this, std::placeholders::_1));
        SceneObject::getDestroyedEvent().addListener(std::bind(&Hierarchy::onSceneObjectDeleted, this, std::placeholders::_1));
        SceneObject::getAttachedEvent().addListener(std::bind(&Hierarchy::onSceneObjectAttached, this, std::placeholders::_1));
        SceneObject::getDetachedEvent().addListener(std::bind(&Hierarchy::onSceneObjectDetached, this, std::placeholders::_1));        
        Scene::getTargetAddedEvent().addListener(std::bind(&Hierarchy::onTargetAdded, this, std::placeholders::_1));
        Scene::getTargetRemovedEvent().addListener(std::bind(&Hierarchy::onTargetRemoved, this, std::placeholders::_1));
        Scene::getTargetClearedEvent().addListener(std::bind(&Hierarchy::onTargetCleared, this));
    }

    Hierarchy::~Hierarchy()
    {
        SceneObject::getCreatedEvent().removeAllListeners();
        SceneObject::getDestroyedEvent().removeAllListeners();
        SceneObject::getAttachedEvent().removeAllListeners();
        SceneObject::getDetachedEvent().removeAllListeners();
        Scene::getTargetAddedEvent().removeAllListeners();
        Scene::getTargetRemovedEvent().removeAllListeners();
        Scene::getTargetClearedEvent().removeAllListeners();
        clear();
        m_groupLayout = nullptr;
    }

    void Hierarchy::onSceneObjectCreated(SceneObject& sceneObject)
    {
        auto objId = sceneObject.getId();
        auto name = sceneObject.getName();
        auto isGuiObj = sceneObject.isGUIObject();
        auto node = createWidget<TreeNode>(sceneObject.getName(), false, sceneObject.getChildren().size() == 0);
        node->getOnClickEvent().addListener([objId, this](auto widget) {
            auto object = Editor::getCurrentScene()->findObjectById(objId);
            if (object)
            {
                auto keyModifier = Editor::getApp()->getInputHandler()->getKeyboard()->getKeyModifier();
                if (keyModifier & (int)KeyModifier::KEY_MOD_CTRL)
                {
                    if (object->isSelected())
                        Editor::getCurrentScene()->removeTarget(object.get());
                    else
                        Editor::getCurrentScene()->addTarget(object.get(), false);
                }
                else if (keyModifier & (int)KeyModifier::KEY_MOD_SHIFT)
                {
                    auto firstTarget = Editor::getCurrentScene()->getFirstTarget();
                    if (firstTarget == nullptr)
                    {
                        Editor::getCurrentScene()->addTarget(object.get(), false);
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
                                if (obj) Editor::getCurrentScene()->removeTarget(obj.get());
                            }
                            for (auto it = fistIdxPair; it != m_objectNodeMap.end(); ++it)
                            {
                                auto obj = Editor::getCurrentScene()->findObjectById(it->first);
                                if (obj) Editor::getCurrentScene()->addTarget(obj.get(), false);
                                if (it == currIdxPair) break;
                            }
                            for (auto it = currIdxPair; it != m_objectNodeMap.end(); ++it)
                            {
                                if (it == currIdxPair) continue;
                                auto obj = Editor::getCurrentScene()->findObjectById(it->first);
                                if (obj) Editor::getCurrentScene()->removeTarget(obj.get());
                            }
                        }
                    }
                }
                else
                {
                    Editor::getCurrentScene()->addTarget(object.get(), true);
                }
            }
        });
        node->addPlugin<DDTargetPlugin<uint64_t>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([this, objId](auto txt) {           
            auto parent = Editor::getCurrentScene()->findObjectById(objId);
            auto loop = false;
            for (auto& target : Editor::getCurrentScene()->getTargets())
            {
                if (target && parent->isRelative(target->getId()))
                {
                    loop = true; break;
                }
            }
            if (!loop)
            {
                std::vector<SceneObject*> movingObjects = {};
                for (auto& target : Editor::getCurrentScene()->getTargets())
                {
                    if (target && target->getParent()->isSelected() == false)
                        movingObjects.push_back(target);
                }
                for (auto& target : movingObjects)
                {
                    target->setParent(parent.get());
                }
            }
        });
        node->addPlugin<DDTargetPlugin<std::string>>(GetFileExtensionSuported(E_FileExts::Prefab)[0])->getOnDataReceivedEvent().addListener([this, objId](auto path) {
            Editor::getCurrentScene()->loadPrefab(objId, path);
        });
        node->addPlugin<DDSourcePlugin<uint64_t>>(EDragDropID::OBJECT, sceneObject.getName(), objId);

        auto ctxMenu = node->addPlugin<ContextMenu>(sceneObject.getName() + "_Context");
        addCreationContextMenu(ctxMenu);

        m_objectNodeMap[objId] = node;
    }

    void Hierarchy::onSceneObjectDeleted(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());
        if (nodePair != m_objectNodeMap.end())
        {
            if(nodePair->second->hasContainer())
                nodePair->second->getContainer()->removeWidget(nodePair->second);
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

            if (sceneObject.getParent())
            {
                auto parentWidget = m_objectNodeMap.at(sceneObject.getParent()->getId());
                parentWidget->setIsLeaf(false);
                parentWidget->addWidget(widget);
                parentWidget->open();
            }
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

    void Hierarchy::onSceneObjectChangedName(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());
        if (nodePair != m_objectNodeMap.end())
            nodePair->second->setName(sceneObject.getName());
    }

    void Hierarchy::onTargetAdded(SceneObject* object)
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

        if (Editor::getCanvas())
        {
            Editor::getCanvas()->setTargetObject(object);
        }
    }

    void Hierarchy::onTargetRemoved(SceneObject* object)
    {
        if (!object) return;
        auto nodePair = m_objectNodeMap.find(object->getId());
        if (nodePair != m_objectNodeMap.end())
        {
            nodePair->second->setIsSelected(false);
            object->getNameChangedEvent().removeAllListeners();
        }

        if (Editor::getCanvas())
        {
            auto targets = Editor::getCurrentScene()->getTargets();
            if (!targets.empty() && targets[0])
                Editor::getCanvas()->setTargetObject(object);
            else
                Editor::getCanvas()->setTargetObject(nullptr);
        }
    }

    void Hierarchy::setNodeHighlight(uint64_t nodeId, bool highlight)
    {
        auto nodePair = m_objectNodeMap.find(nodeId);
        if (nodePair != m_objectNodeMap.end())
        {
            nodePair->second->setHighlighted(highlight);

            if (highlight)
            {
                m_highlightTimer.setTimeOut<uint64_t, bool>(1000, std::bind(&Hierarchy::setNodeHighlight, this, std::placeholders::_1, std::placeholders::_2), nodeId, false);
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
    }

    void Hierarchy::addCreationContextMenu(std::shared_ptr<ContextMenu>& ctxMenu)
    {
        if (ctxMenu == nullptr || Editor::getCurrentScene() == nullptr)
            return;

        auto createMenu = ctxMenu->createWidget<Menu>("Create");
        createMenu->createWidget<MenuItem>("New Object")->getOnClickEvent().addListener([](auto widget) {
            auto targets = Editor::getCurrentScene()->getTargets();
            auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
            auto newObject = Editor::getCurrentScene()->createObject("New Object", currentObject);
            Editor::getCurrentScene()->addTarget(newObject.get(), true);
        });

        // Camera
        createMenu->createWidget<MenuItem>("Camera")->getOnClickEvent().addListener([](auto widget) {
            auto targets = Editor::getCurrentScene()->getTargets();
            auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
            auto newObject = Editor::getCurrentScene()->createObject("Camera", currentObject);
            newObject->addComponent<CameraComponent>("camera");
            newObject->addComponent<FigureComponent>(GetEnginePath("figures/camera.pyxf"))->setSkipSerialize(true);
            Editor::getCurrentScene()->addTarget(newObject.get(), true);
        });

        // Primitives
        {
            auto shapeMenu = createMenu->createWidget<Menu>("Primitive");
            shapeMenu->createWidget<MenuItem>("Cube")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Cube", currentObject);
                newObject->addComponent<FigureComponent>("figures/cube.pyxf");
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            shapeMenu->createWidget<MenuItem>("Plane")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Plane", currentObject);
                newObject->addComponent<FigureComponent>("figures/plane.pyxf");
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            shapeMenu->createWidget<MenuItem>("Sphere")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Sphere", currentObject);
                newObject->addComponent<FigureComponent>("figures/sphere.pyxf");
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            shapeMenu->createWidget<MenuItem>("Cone")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Cone", currentObject);
                newObject->addComponent<FigureComponent>("figures/cone.pyxf");
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            shapeMenu->createWidget<MenuItem>("Cylinder")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Cylinder", currentObject);
                newObject->addComponent<FigureComponent>("figures/cylinder.pyxf");
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            shapeMenu->createWidget<MenuItem>("Torus")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Torus", currentObject);
                newObject->addComponent<FigureComponent>("figures/torus.pyxf");
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });
        }

        // Lights
        {
            auto lightMenu = createMenu->createWidget<Menu>("Light");
            lightMenu->createWidget<MenuItem>("Directional Light")->getOnClickEvent().addListener([](auto widget) {
                if (!Editor::getCurrentScene()->isDirectionalLightAvailable())
                {
                    auto msgBox = MsgBox("Error", "Max number of Directional Light reached!", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::error);
                    while (!msgBox.ready(1000));
                    return;
                }
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Directional Light", currentObject);
                newObject->addComponent<DirectionalLight>();
                newObject->getTransform()->setPosition({ 0.f, 5.f, 0.f });
                newObject->getTransform()->setRotation({ DEGREES_TO_RADIANS(90.f), 0.f, .0f });
                newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/direct-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            lightMenu->createWidget<MenuItem>("Point Light")->getOnClickEvent().addListener([](auto widget) {
                if (!Editor::getCurrentScene()->isPointLightAvailable())
                {
                    auto msgBox = MsgBox("Error", "Max number of Point Light reached!", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::error);
                    while (!msgBox.ready(1000));
                    return;
                }
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Point Light", currentObject);
                newObject->addComponent<PointLight>();
                newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/point-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            lightMenu->createWidget<MenuItem>("Spot Light")->getOnClickEvent().addListener([](auto widget) {
                if (!Editor::getCurrentScene()->isSpotLightAvailable())
                {
                    auto msgBox = MsgBox("Error", "Max number of Spot Light reached!", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::error);
                    while (!msgBox.ready(1000));
                    return;
                }
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Spot Light", currentObject);
                newObject->addComponent<SpotLight>();
                newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/spot-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });
        }

        // Audio
        {
            auto audioMenu = createMenu->createWidget<Menu>("Audio");
            audioMenu->createWidget<MenuItem>("Audio Source")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Audio Source", currentObject, true);
                newObject->addComponent<AudioSource>();
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            audioMenu->createWidget<MenuItem>("Audio Listener")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Audio Listener", currentObject, true);
                newObject->addComponent<AudioListener>();
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });
        }

        // Effects
        {
            auto effectMenu = createMenu->createWidget<Menu>("Effect");
            effectMenu->createWidget<MenuItem>("Particle")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("Particle", currentObject, true);
                newObject->addComponent<Particle>();
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });
        }

        // GUI
        {
            auto guiMenu = createMenu->createWidget<Menu>("GUI");

            guiMenu->createWidget<MenuItem>("UIImage")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("UIImage", currentObject, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIImage>("sprites/rect", rect->getSize());
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            guiMenu->createWidget<MenuItem>("UIText")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("UIText", currentObject, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIText>("Text");
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            guiMenu->createWidget<MenuItem>("UITextField")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("UITextField", currentObject, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIImage>("sprites/rect", rect->getSize());
                auto newLabel = Editor::getCurrentScene()->createObject("Label", newObject, true, Vec2());
                newLabel->addComponent<UITextField>("TextField");                
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            guiMenu->createWidget<MenuItem>("UIButton")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("UIButton", currentObject, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIButton>("sprites/rect", rect->getSize());
                Editor::getCurrentScene()->createObject("Label", newObject, true, Vec2())->addComponent<UIText>("Button");
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });

            guiMenu->createWidget<MenuItem>("UISlider")->getOnClickEvent().addListener([](auto widget) {
                auto targets = Editor::getCurrentScene()->getTargets();
                auto currentObject = (!targets.empty() && targets[0]) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                auto newObject = Editor::getCurrentScene()->createObject("UISlider", currentObject, true, Vec2(160.f, 40.f));
                // Create Slider
                auto sliderComp = newObject->addComponent<UISlider>();
                // Create Background
                auto newBG = Editor::getCurrentScene()->createObject("background", newObject, true, Vec2(160.f, 16.f));
                auto rectBG = std::dynamic_pointer_cast<RectTransform>(newBG->getTransform());
                auto imgBG = newBG->addComponent<UIImage>("sprites/rect", rectBG->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
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
                auto imgFill = newFill->addComponent<UIImage>("sprites/rect", rectFill->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
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
                newHandle->addComponent<UIImage>("sprites/rect", rectHandle->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                newHandle->setIsRaycastTarget(false);
                sliderComp->setFillObject(newFill);
                sliderComp->setHandleObject(newHandle);
                Editor::getCurrentScene()->addTarget(newObject.get(), true);
            });
        }
        ctxMenu->createWidget<MenuItem>("Copy")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->copyObject();
            });
        });
        ctxMenu->createWidget<MenuItem>("Paste")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->pasteObject();
            });
        });
        ctxMenu->createWidget<MenuItem>("Duplicate")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->cloneObject();
            });
        });
        ctxMenu->createWidget<MenuItem>("Delete")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->deleteObject();
            });
        });
    }

    void Hierarchy::initialize()
    {
        if (!m_bInitialized)
        {
            m_groupLayout = createWidget<Group>("Hierarchy_Group", false, false);
            auto ctxMenu = m_groupLayout->addPlugin<WindowContextMenu>("Hierarchy_Context");
            addCreationContextMenu(std::dynamic_pointer_cast<ContextMenu>(ctxMenu));
            m_bInitialized = true;
        }
    }

    void Hierarchy::drawWidgets()
    {
        // Draw widgets
        Panel::drawWidgets();

        if (m_bIsOpened && ImGui::IsWindowFocused() && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            auto vMin = ImGui::GetWindowContentRegionMin();
            auto vMax = ImGui::GetWindowContentRegionMax();
            
            vMin.y += 6; // padding

            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;
            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;

            float nodeMapHeight = k_nodeDefaultHeight;
            for (auto item : m_objectNodeMap) {
                if (item.second && item.second->isOpened())
                    nodeMapHeight += k_nodeDefaultHeight;
            }
            vMin.y += nodeMapHeight;

            if (ImGui::GetMousePos().x >= vMin.x && ImGui::GetMousePos().x <= vMax.x
                && ImGui::GetMousePos().y >= vMin.y && ImGui::GetMousePos().y <= vMax.y) {
                if(Editor::getCurrentScene())
                    Editor::getCurrentScene()->clearTargets();
            }
        }
    }

    void Hierarchy::clear()
    {
        if (m_groupLayout)
        {
            m_groupLayout->removeAllWidgets();
            m_groupLayout->removeAllPlugins();
        }
        m_groupLayout = nullptr;
        removeAllWidgets();
        m_objectNodeMap.clear();
        m_bInitialized = false;
    }
}
