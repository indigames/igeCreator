#include <imgui.h>
#include <imgui_internal.h>

#include "core/filesystem/FileSystem.h"

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
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/gui/UIImage.h"
#include "components/gui/UIText.h"
#include "components/gui/UITextField.h"
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
        SceneObject::getNameChangedEvent().addListener(std::bind(&Hierarchy::onSceneObjectChangedName, this, std::placeholders::_1));
        SceneObject::getSelectedEvent().addListener(std::bind(&Hierarchy::onSceneObjectSelected, this, std::placeholders::_1));
    }

    Hierarchy::~Hierarchy()
    {
        SceneObject::getCreatedEvent().removeAllListeners();
        SceneObject::getDestroyedEvent().removeAllListeners();
        SceneObject::getAttachedEvent().removeAllListeners();
        SceneObject::getDetachedEvent().removeAllListeners();
        SceneObject::getNameChangedEvent().removeAllListeners();
        SceneObject::getSelectedEvent().removeAllListeners();
        clear();
        m_groupLayout = nullptr;
    }

    void Hierarchy::onSceneObjectCreated(SceneObject& sceneObject)
    {
        auto objId = sceneObject.getId();
        auto name = sceneObject.getName();
        auto isGuiObj = sceneObject.isGUIObject();
        auto node = createWidget<TreeNode>(sceneObject.getName(), false, sceneObject.getChildrenCount() == 0);
        node->getOnClickEvent().addListener([objId, this](auto widget) {
            TaskManager::getInstance()->addTask([objId, this](){
                // Set previous selected to false
                auto nodePair = m_objectNodeMap.find(m_selectedNodeId);
                if (nodePair != m_objectNodeMap.end())
                {
                    nodePair->second->setIsSelected(false);
                }

                // Update current selected id
                nodePair = m_objectNodeMap.find(objId);
                if (nodePair != m_objectNodeMap.end())
                {
                    m_selectedNodeId = objId;
                    nodePair->second->setIsSelected(true);
                }

                // Update selected object
                auto object = Editor::getCurrentScene()->findObjectById(objId);
                if(object) object->setSelected(true);
            });
        });
        node->addPlugin<DDTargetPlugin<uint64_t>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([this, objId](auto txt) {
            auto currentObject = Editor::getCurrentScene()->findObjectById(txt);
            auto obj = Editor::getCurrentScene()->findObjectById(objId);
            if (currentObject->getParent())
                currentObject->getParent()->removeChild(currentObject);
            obj->addChild(currentObject);
        });
        node->addPlugin<DDSourcePlugin<uint64_t>>(EDragDropID::OBJECT, sceneObject.getName(), objId);

        auto ctxMenu = node->addPlugin<ContextMenu>(sceneObject.getName() + "_Context");
        auto createMenu = ctxMenu->createWidget<Menu>("Create");
        if (isGuiObj)
        {
            createMenu->createWidget<MenuItem>("Button")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newBtn = Editor::getCurrentScene()->createGUIObject("Button", currentObject);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newBtn->getTransform());
                newBtn->addComponent<UIImage>("sprite/rect", rect->getSize());
                auto newBtnLabel = Editor::getCurrentScene()->createGUIObject("Label", newBtn, Vec3(), Vec2());
                newBtnLabel->addComponent<UIText>("Button");
                newBtn->addComponent<ScriptComponent>(fs::createScript(newBtn->getName() + std::to_string(newBtn->getId())));
                newBtn->setSelected(true);
            });

            createMenu->createWidget<MenuItem>("UIImage")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createGUIObject("UIImage", currentObject);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIImage>("sprite/rect", rect->getSize());
                newObject->setSelected(true);
            });

            createMenu->createWidget<MenuItem>("UIText")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createGUIObject("UIText", currentObject);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIText>("Text");
                newObject->setSelected(true);
            });
            
            createMenu->createWidget<MenuItem>("UITextField")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createGUIObject("UITextField", currentObject);
                auto rect = std::dynamic_pointer_cast<RectTransform>(newObject->getTransform());
                newObject->addComponent<UIImage>("sprite/rect", rect->getSize());
                auto newLabel = Editor::getCurrentScene()->createGUIObject("Label", newObject, Vec3(), Vec2());
                newLabel->addComponent<UITextField>("TextField");
                auto id = newLabel->getId();
                newObject->getSelectedEvent().addListener([id](SceneObject& obj) {
                    auto label = obj.findObjectById(id);
                    if (label) {
                        auto txtField = label->getComponent<UITextField>();
                        if(txtField) {
                            txtField->onClick();
                        }
                    }
                });
                newObject->setSelected(true);
            });
        }
        else
        {
            createMenu->createWidget<MenuItem>("New Object")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createObject("New Object", currentObject);
                newObject->setSelected(true);
            });

            auto shapeMenu = createMenu->createWidget<Menu>("Primitives");
            shapeMenu->createWidget<MenuItem>("Cube")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createObject("Cube", currentObject);
                newObject->addComponent<FigureComponent>("figure/cube.pyxf");
                newObject->setSelected(true);
            });

            shapeMenu->createWidget<MenuItem>("Plane")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createObject("Plane", currentObject);
                newObject->addComponent<FigureComponent>("figure/plane.pyxf");
                newObject->setSelected(true);
            });

            shapeMenu->createWidget<MenuItem>("Sphere")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createObject("Sphere", currentObject);
                newObject->addComponent<FigureComponent>("figure/sphere.pyxf");
                newObject->setSelected(true);
            });

            shapeMenu->createWidget<MenuItem>("Cone")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createObject("Cone", currentObject);
                newObject->addComponent<FigureComponent>("figure/cone.pyxf");
                newObject->setSelected(true);
            });

            shapeMenu->createWidget<MenuItem>("Cylinder")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createObject("Cylinder", currentObject);
                newObject->addComponent<FigureComponent>("figure/cylinder.pyxf");
                newObject->setSelected(true);
            });

            shapeMenu->createWidget<MenuItem>("Torus")->getOnClickEvent().addListener([objId](auto widget) {
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getCurrentScene()->createObject("Torus", currentObject);
                newObject->addComponent<FigureComponent>("figure/Torus.pyxf");
                newObject->setSelected(true);
            });
        }

        ctxMenu->createWidget<MenuItem>("Delete")->getOnClickEvent().addListener([objId](auto widget) {
            TaskManager::getInstance()->addTask([objId](){
                auto currentObject = Editor::getCurrentScene()->findObjectById(objId);
                if (currentObject && currentObject->getParent()) currentObject->getParent()->setSelected(true);
                Editor::getInstance()->setSelectedObject(-1);
                Editor::getCurrentScene()->removeObjectById(objId);
            });
        });
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

            if (sceneObject.hasParent())
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
            if (sceneObject.hasParent() && sceneObject.getParent()->getChildrenCount() == 1)
            {
                auto parentWidget = m_objectNodeMap.at(sceneObject.getParent()->getId());
                parentWidget->setIsLeaf(true);
            }

            auto widget = nodePair->second;
            if (widget->hasContainer())
                widget->getContainer()->removeWidget(widget);
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
            Editor::getInstance()->setSelectedObject(sceneObject.getId());
        }
    }

    void Hierarchy::initialize()
    {
        if (!m_bInitialized)
        {
            m_groupLayout = createWidget<Group>("Hierarchy_Group", false, false);
            auto ctxMenu = m_groupLayout->addPlugin<WindowContextMenu>("Hierarchy_Context");
            ctxMenu->createWidget<MenuItem>("New Scene")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([]() {
                    if (Editor::getCurrentScene() == nullptr)
                    {
                        auto scene = Editor::getSceneManager()->createScene("New scene");
                        Editor::getSceneManager()->setCurrentScene(scene);
                        Editor::getCurrentScene()->findObjectById(0)->setSelected(true);
                    }
                    auto newObj = Editor::getCurrentScene()->createObject("New scene");
                    newObj->setSelected(true);
                });
            });

            ctxMenu->createWidget<MenuItem>("New Canvas")->getOnClickEvent().addListener([](auto widget) {
                TaskManager::getInstance()->addTask([](){
                    if (Editor::getCurrentScene() == nullptr)
                    {
                        auto scene = Editor::getSceneManager()->createScene("New scene");
                        Editor::getSceneManager()->setCurrentScene(scene);
                    }
                    auto newObj = Editor::getCurrentScene()->createGUIObject("Canvas");
                    auto canvas = newObj->getComponent<ige::scene::Canvas>();
                    canvas->setDesignCanvasSize(Vec2(540.f, 960.f));
                    auto uiImage = newObj->addComponent<UIImage>();
                    uiImage->setPath("sprite/rect");
                    newObj->setSelected(true);
                });
            });

            m_bInitialized = true;
        }
    }

    void Hierarchy::drawWidgets()
    {
        // Show FPS
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // Draw widgets
        Panel::drawWidgets();
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
