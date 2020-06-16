#include <imgui.h>
#include <imgui_internal.h>

#include <scene/Scene.h>
#include <scene/SceneObject.h>
#include "core/panels/Hierarchy.h"
#include "core/Editor.h"
#include "core/menu/ContextMenu.h"
#include "core/menu/MenuItem.h"

using namespace ige::scene;

namespace ige::creator
{
    Hierarchy::Hierarchy(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        m_selectedNode = nullptr;
        SceneObject::getCreatedEvent().addListener(std::bind(&Hierarchy::onSceneObjectCreated, this, std::placeholders::_1));
        SceneObject::getDestroyedEvent().addListener(std::bind(&Hierarchy::onSceneObjectDeleted, this, std::placeholders::_1));
        SceneObject::getAttachedEvent().addListener(std::bind(&Hierarchy::onSceneObjectAttached, this, std::placeholders::_1));
        SceneObject::getDetachedEvent().addListener(std::bind(&Hierarchy::onSceneObjectDetached, this, std::placeholders::_1));
    }

    Hierarchy::~Hierarchy()
    {
        SceneObject::getCreatedEvent().removeAllListeners();
        SceneObject::getDestroyedEvent().removeAllListeners();
        SceneObject::getAttachedEvent().removeAllListeners();
        SceneObject::getDetachedEvent().removeAllListeners();

        for (auto& widget : m_widgets)
            widget->getOnClickEvent().removeAllListeners();

        m_objectNodeMap.clear();
    }

    void Hierarchy::onSceneObjectCreated(SceneObject& sceneObject)
    {
        auto node = createWidget<TreeNode>(sceneObject.getName(), false, sceneObject.getChildrenCount() == 0);
        m_objectNodeMap[sceneObject.getName()] = node;
        node->getOnClickEvent().addListener([node, &sceneObject, this]() {
            if (m_selectedNode != nullptr && m_selectedNode != node) 
            {
                m_selectedNode->setIsSelected(false);
            }
            m_selectedNode = node;
            m_selectedNode->setIsSelected(true);
            Editor::getInstance()->setSelectedObject(sceneObject.getName());
        });

        auto ctxMenu = node->createWidget<ContextMenu>("Hierarchy Context");
        auto createMenu = ctxMenu->createWidget<Menu>("Create");
        createMenu->createWidget<MenuItem>("Cone");
        createMenu->createWidget<MenuItem>("Cube");
        createMenu->createWidget<MenuItem>("Cylinder");
        createMenu->createWidget<MenuItem>("Plane");
        createMenu->createWidget<MenuItem>("Sphere");

        ctxMenu->createWidget<MenuItem>("Delete")->getOnClickEvent().addListener([node, &sceneObject, this]() {
            Editor::getInstance()->getSceneManager()->getCurrentScene()->removeObjectById(sceneObject.getId());
        }); 
    }

    void Hierarchy::onSceneObjectDeleted(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getName());
        if (nodePair != m_objectNodeMap.end())
        {
            removeWidget(nodePair->second);
            m_objectNodeMap.erase(nodePair);
        }
    }

    void Hierarchy::onSceneObjectAttached(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getName());

        if (nodePair != m_objectNodeMap.end())
        {
            auto widget = nodePair->second;

            if (widget->hasContainer())
                 widget->getContainer()->removeWidget(widget);

            if (sceneObject.hasParent())
            {
                auto parentWidget = m_objectNodeMap.at(sceneObject.getParent()->getName());
                parentWidget->setIsLeaf(false);
                parentWidget->addWidget(widget);
            }
        }
    }

    void Hierarchy::onSceneObjectDetached(SceneObject& sceneObject)
    {
        auto nodePair = m_objectNodeMap.find(sceneObject.getName());

        if (nodePair != m_objectNodeMap.end())
        {            
            if (sceneObject.hasParent() && sceneObject.getParent()->getChildrenCount() == 1)
            {
                auto parentWidget = m_objectNodeMap.at(sceneObject.getParent()->getName());
                parentWidget->setIsLeaf(true);
            }

            auto widget = nodePair->second;
            if (widget->hasContainer())
                widget->getContainer()->removeWidget(widget);

            m_objectNodeMap.erase(nodePair);
        }
    }

    void Hierarchy::initialize()
    {
        clear();
    }

    void Hierarchy::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void Hierarchy::clear()
    {
    }
}
