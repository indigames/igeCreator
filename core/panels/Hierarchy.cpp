#include <imgui.h>
#include <imgui_internal.h>

#include <scene/Scene.h>
#include <scene/SceneObject.h>
#include "core/panels/Hierarchy.h"
#include "core/Editor.h"
#include "core/menu/ContextMenu.h"
#include "core/menu/MenuItem.h"
#include "core/task/TaskManager.h"

using namespace ige::scene;

#include <utils/PyxieHeaders.h>

namespace ige::creator
{
    Hierarchy::Hierarchy(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings), m_targetObject(nullptr)
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

        clear();
    }

    void Hierarchy::setTargetObject(const std::shared_ptr<SceneObject>& obj)
    {
        m_targetObject = obj;
    }

    void Hierarchy::onSceneObjectCreated(SceneObject& sceneObject)
    {
        auto objId = sceneObject.getId();
        auto node = createWidget<TreeNode>(sceneObject.getName(), false, sceneObject.getChildrenCount() == 0, sceneObject.getName() == "root");
        node->getOnClickEvent().addListener([objId, this]() {
            TaskManager::getInstance()->getTaskflow().emplace([objId, this]() {
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
                    Editor::getInstance()->setSelectedObject(objId);
                }
            });
        });
        auto ctxMenu = node->addPlugin<ContextMenu>("Hierarchy_Context");
        auto createMenu = ctxMenu->createWidget<Menu>("Create");
        createMenu->createWidget<MenuItem>("New Object")->getOnClickEvent().addListener([objId]() {
            TaskManager::getInstance()->getTaskflow().emplace([objId]() {
                auto currentObject = Editor::getSceneManager()->getCurrentScene()->findObjectById(objId);
                auto newObject = Editor::getSceneManager()->getCurrentScene()->createObject("New Object", currentObject);
                });
            });

        auto shapeMenu = createMenu->createWidget<Menu>("Primitives");
        shapeMenu->createWidget<MenuItem>("Cone");
        shapeMenu->createWidget<MenuItem>("Cube");
        shapeMenu->createWidget<MenuItem>("Cylinder");
        shapeMenu->createWidget<MenuItem>("Plane");
        shapeMenu->createWidget<MenuItem>("Sphere");

        ctxMenu->createWidget<MenuItem>("Delete")->getOnClickEvent().addListener([objId]() {
            TaskManager::getInstance()->getTaskflow().emplace([objId]() {
                Editor::getInstance()->setSelectedObject(0);
                if (objId != 0) Editor::getSceneManager()->getCurrentScene()->removeObjectById(objId);
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
        if (m_targetObject)
        {
            m_targetObject->setSelected(false);
        }
       
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
    
    void Hierarchy::drawWidgets()
    {
        // Show FPS
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // Draw widgets
        Panel::drawWidgets();
    }

    void Hierarchy::clear()
    {
        m_targetObject = nullptr;
    }
}
