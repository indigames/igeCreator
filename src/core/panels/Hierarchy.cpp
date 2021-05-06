#include <imgui.h>
#include <imgui_internal.h>

#include "core/filesystem/FileSystem.h"
#include "core/FileHandle.h"
#include "core/dialog/MsgBox.h"

#include <scene/Scene.h>
#include <scene/SceneObject.h>
#include "core/panels/Hierarchy.h"
#include "core/Editor.h"
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
        SceneObject::getSelectedEvent().addListener(std::bind(&Hierarchy::onSceneObjectSelected, this, std::placeholders::_1));
        SceneObject::getDeselectedEvent().addListener(std::bind(&Hierarchy::onSceneObjectDeselected, this, std::placeholders::_1));
    }

    Hierarchy::~Hierarchy()
    {
        SceneObject::getCreatedEvent().removeAllListeners();
        SceneObject::getDestroyedEvent().removeAllListeners();
        SceneObject::getAttachedEvent().removeAllListeners();
        SceneObject::getDetachedEvent().removeAllListeners();
        SceneObject::getSelectedEvent().removeAllListeners();
        SceneObject::getDeselectedEvent().removeAllListeners();
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
            if (object) object->setSelected(true);
            else pyxie_printf("Cant find Id %ld \n", objId);
        });

        //! Update List Content Size 
        node->getOnClosedEvent().addListener([objId, this]() {
            auto object = Editor::getCurrentScene()->findObjectById(objId);
            this->onSceneObjectCollapse(object, true);

        });
        node->getOnOpenedEvent().addListener([objId, this]() {
            auto object = Editor::getCurrentScene()->findObjectById(objId);
            this->onSceneObjectCollapse(object, false);
        });

        node->addPlugin<DDTargetPlugin<uint64_t>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([this, objId](auto txt) {
            auto currentObject = Editor::getCurrentScene()->findObjectById(txt);
            auto obj = Editor::getCurrentScene()->findObjectById(objId);
            currentObject->setParent(obj.get());
        });
        node->addPlugin<DDTargetPlugin<std::string>>(GetFileExtensionSuported(E_FileExts::Prefab)[0])->getOnDataReceivedEvent().addListener([this, objId](auto path) {
            Editor::getCurrentScene()->loadPrefab(objId, path);
        });
        node->addPlugin<DDSourcePlugin<uint64_t>>(EDragDropID::OBJECT, sceneObject.getName(), objId);

        auto ctxMenu = node->addPlugin<ContextMenu>(sceneObject.getName() + "_Context");
        addCreationContextMenu(ctxMenu);

        ctxMenu->createWidget<MenuItem>("Delete")->getOnClickEvent().addListener([objId](auto widget) {
            TaskManager::getInstance()->addTask([objId](){
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                if (currentObject && currentObject->getParent()) currentObject->getParent()->setSelected(true);
                Editor::getInstance()->setSelectedObject(-1);
                Editor::getCurrentScene()->removeObjectById(objId);
            });
        });
        m_objectNodeMap[objId] = node;

        m_NodeCollapseMap[objId] = true;
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
        m_NodeCollapseMap.erase(sceneObject.getId());
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
                auto parentWidget = m_objectNodeMap.at(sceneObject.getParent()->getId());
                parentWidget->setIsLeaf(true);

                auto widget = nodePair->second;
                if (widget->hasContainer())
                    widget->getContainer()->removeWidget(widget);
            }
        }
    }

    void Hierarchy::onSceneObjectChangedName(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());
        if (nodePair != m_objectNodeMap.end())
            nodePair->second->setName(sceneObject.getName());
    }

    void Hierarchy::onSceneObjectSelected(SceneObject& sceneObject)
    {
        // Set previous selected to false
        auto nodePair = m_objectNodeMap.find(m_selectedNodeId);
        if (nodePair != m_objectNodeMap.end())
        {
            nodePair->second->setIsSelected(false);
        }

        // Update current selected id
        nodePair = m_objectNodeMap.find(sceneObject.getId());
        if (nodePair != m_objectNodeMap.end())
        {
            m_selectedNodeId = sceneObject.getId();
            nodePair->second->setIsSelected(true);

            // Recursive open parent nodes
            auto parent = sceneObject.getParent();
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

            auto oldObject = Editor::getInstance()->getSelectedObject();
            if (oldObject)
            {
                oldObject->getNameChangedEvent().removeAllListeners();
            }
            sceneObject.getNameChangedEvent().addListener(std::bind(&Hierarchy::onSceneObjectChangedName, this, std::placeholders::_1));

            Editor::getInstance()->setSelectedObject(sceneObject.getId());
        }
    }

    void Hierarchy::onSceneObjectDeselected(SceneObject& sceneObject)
    {
        // Update current selected id
        auto nodePair = m_objectNodeMap.find(sceneObject.getId());
        if (nodePair != m_objectNodeMap.end())
        {
            m_selectedNodeId = sceneObject.getId();
            nodePair->second->setIsSelected(false);
            sceneObject.getNameChangedEvent().removeAllListeners();
            Editor::getInstance()->setSelectedObject(-1);
        }
    }

    void Hierarchy::addCreationContextMenu(std::shared_ptr<ContextMenu>& ctxMenu)
    {
        if (ctxMenu == nullptr)
            return;

        auto createMenu = ctxMenu->createWidget<Menu>("Create");
        createMenu->createWidget<MenuItem>("New Object")->getOnClickEvent().addListener([](auto widget) {
            auto currentObject = Editor::getInstance()->getSelectedObject();
            auto newObject = Editor::getCurrentScene()->createObject("New Object", currentObject);
            newObject->setSelected(true);
        });

        // Camera
        createMenu->createWidget<MenuItem>("Camera")->getOnClickEvent().addListener([](auto widget) {
            auto currentObject = Editor::getInstance()->getSelectedObject();
            auto newObject = Editor::getCurrentScene()->createObject("Camera", currentObject);
            newObject->addComponent<CameraComponent>("camera");
            newObject->addComponent<FigureComponent>(GetEnginePath("figures/camera.pyxf"))->setSkipSerialize(true);
        });

        // Primitives
        {
            auto shapeMenu = createMenu->createWidget<Menu>("Primitive");
            shapeMenu->createWidget<MenuItem>("Cube")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Cube", currentObject);
                newObject->addComponent<FigureComponent>("figures/cube.pyxf");
            });

            shapeMenu->createWidget<MenuItem>("Plane")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Plane", currentObject);
                newObject->addComponent<FigureComponent>("figures/plane.pyxf");
            });

            shapeMenu->createWidget<MenuItem>("Sphere")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Sphere", currentObject);
                newObject->addComponent<FigureComponent>("figures/sphere.pyxf");
            });

            shapeMenu->createWidget<MenuItem>("Cone")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Cone", currentObject);
                newObject->addComponent<FigureComponent>("figures/cone.pyxf");
            });

            shapeMenu->createWidget<MenuItem>("Cylinder")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Cylinder", currentObject);
                newObject->addComponent<FigureComponent>("figures/cylinder.pyxf");
            });

            shapeMenu->createWidget<MenuItem>("Torus")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Torus", currentObject);
                newObject->addComponent<FigureComponent>("figures/torus.pyxf");
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
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Directional Light", currentObject);
                newObject->addComponent<DirectionalLight>();
                newObject->getTransform()->setPosition({ 0.f, 5.f, 0.f });
                newObject->getTransform()->setRotation({ DEGREES_TO_RADIANS(90.f), 0.f, .0f });
                newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/direct-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
            });

            lightMenu->createWidget<MenuItem>("Point Light")->getOnClickEvent().addListener([](auto widget) {
                if (!Editor::getCurrentScene()->isPointLightAvailable())
                {
                    auto msgBox = MsgBox("Error", "Max number of Point Light reached!", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::error);
                    while (!msgBox.ready(1000));
                    return;
                }
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Point Light", currentObject);
                newObject->addComponent<PointLight>();
                newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/point-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
            });

            lightMenu->createWidget<MenuItem>("Spot Light")->getOnClickEvent().addListener([](auto widget) {
                if (!Editor::getCurrentScene()->isSpotLightAvailable())
                {
                    auto msgBox = MsgBox("Error", "Max number of Spot Light reached!", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::error);
                    while (!msgBox.ready(1000));
                    return;
                }
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Spot Light", currentObject);
                newObject->addComponent<SpotLight>();
                newObject->addComponent<SpriteComponent>(GetEnginePath("sprites/spot-light"), Vec2(0.5f, 0.5f), true)->setSkipSerialize(true);
            });
        }

        // Audio
        {
            auto audioMenu = createMenu->createWidget<Menu>("Audio");
            audioMenu->createWidget<MenuItem>("Audio Source")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Audio Source", currentObject, true);
                newObject->addComponent<AudioSource>();
            });

            audioMenu->createWidget<MenuItem>("Audio Listener")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Audio Listener", currentObject, true);
                newObject->addComponent<AudioListener>();
            });
        }

        // Effects
        {
            auto effectMenu = createMenu->createWidget<Menu>("Effect");
            effectMenu->createWidget<MenuItem>("Particle")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("Particle", currentObject, true);
                newObject->addComponent<Particle>();
            });
        }

        // GUI
        {
            auto guiMenu = createMenu->createWidget<Menu>("GUI");

            guiMenu->createWidget<MenuItem>("UIImage")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("UIImage", currentObject, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIImage>("sprites/rect", rect->getSize());
            });

            guiMenu->createWidget<MenuItem>("UIText")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("UIText", currentObject, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIText>("Text");
            });

            guiMenu->createWidget<MenuItem>("UITextField")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newObject = Editor::getCurrentScene()->createObject("UITextField", currentObject, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIImage>("sprites/rect", rect->getSize());
                auto newLabel = Editor::getCurrentScene()->createObject("Label", newObject, true, Vec2());
                newLabel->addComponent<UITextField>("TextField");
                auto id = newLabel->getId();
                newObject->getSelectedEvent().addListener([id](SceneObject& obj) {
                    auto label = Editor::getCurrentScene()->findObjectById(id);
                    if (label) {
                        auto txtField = label->getComponent<UITextField>();
                        if (txtField) {
                            txtField->onClick();
                        }
                    }
                });
            });

            guiMenu->createWidget<MenuItem>("UIButton")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newBtn = Editor::getCurrentScene()->createObject("UIButton", currentObject, true);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newBtn->getTransform());
                // Create Btn
                newBtn->addComponent<UIButton>("sprites/rect", rect->getSize());
                // Create Label
                auto newBtnLabel = Editor::getCurrentScene()->createObject("Label", newBtn, true, Vec2());
                newBtnLabel->addComponent<UIText>("Button");
            });

            guiMenu->createWidget<MenuItem>("UISlider")->getOnClickEvent().addListener([](auto widget) {
                auto currentObject = Editor::getInstance()->getSelectedObject();
                auto newSlider = Editor::getCurrentScene()->createObject("UISlider", currentObject, true, Vec2(160.f, 40.f));
                // Create Slider
                auto sliderComp = newSlider->addComponent<UISlider>();
                // Create Background
                auto newBG = Editor::getCurrentScene()->createObject("background", newSlider, true, Vec2(160.f, 16.f));
                auto rectBG = std::dynamic_pointer_cast<RectTransform>(newBG->getTransform());
                auto imgBG = newBG->addComponent<UIImage>("sprites/rect", rectBG->getSize(), true, Vec4(10.f, 10.f, 10.f, 10.f));
                imgBG->setColor(0.84f, 0.84f, 0.84f, 1);
                newBG->setIsRaycastTarget(false);
                if (rectBG) {
                    rectBG->setAnchor(Vec4(0.f, 0.f, 1.f, 1.f));
                    rectBG->setAnchoredPosition(Vec2(0, 0));
                }
                // Create Fill
                auto newFillArea = Editor::getCurrentScene()->createObject("fillArea", newSlider, true, Vec2(158.f, 14.f));
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
                auto newHandleArea = Editor::getCurrentScene()->createObject("handleArea", newSlider, true, Vec2(140.f, 30.f));
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
                });
        }
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

            float m_nodeMapHeight = 0;
            for (auto item : m_NodeCollapseMap) {
                if (item.second)
                    m_nodeMapHeight += k_nodeDefaultHeight;
            }
            vMin.y += m_nodeMapHeight;

            if (ImGui::GetMousePos().x >= vMin.x && ImGui::GetMousePos().x <= vMax.x
                && ImGui::GetMousePos().y >= vMin.y && ImGui::GetMousePos().y <= vMax.y) {
                Editor::getInstance()->setSelectedObject(-1);
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

    void Hierarchy::onSceneObjectCollapse(std::shared_ptr<SceneObject> sceneObject, bool IsCollapse) {
        if (sceneObject == nullptr) return;
        for (auto child : sceneObject->getChildren()) {
            auto objId = child->getId();
            m_NodeCollapseMap[objId] = !IsCollapse;
        }
    }
}
