#pragma once

#include <scene/Scene.h>
#include <scene/SceneObject.h>
using namespace ige::scene;

#include "core/Panel.h"
#include "core/widgets/TreeNode.h"
#include "core/layout/Group.h"

namespace ige::creator
{
    class Hierarchy: public Panel
    {
    public:
        Hierarchy(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Hierarchy();

        virtual void clear();
        void setTargetObject(const std::shared_ptr<SceneObject>& obj);

    protected:
        virtual void drawWidgets() override;
        virtual void initialize() override;

        //! Object created/deleted
        void onSceneObjectCreated(SceneObject& sceneObject);
        void onSceneObjectDeleted(SceneObject& sceneObject);

        //! Object attached/detached to its parent
        void onSceneObjectAttached(SceneObject& sceneObject);
        void onSceneObjectDetached(SceneObject& sceneObject);

        //! Object changed name
        void onSceneObjectChangedName(SceneObject& sceneObject);

        //! Object selected
        void onSceneObjectSelected(SceneObject& sceneObject);

        //! Tree node objects
        std::shared_ptr<TreeNode> m_sceneRoot;
        std::unordered_map<uint64_t, std::shared_ptr<TreeNode>> m_objectNodeMap;
        uint64_t m_selectedNodeId = 0xffffffff;

        //! Group layout to add WindowContextMenu
        std::shared_ptr<Group> m_groupLayout;

        //! Selected object
        std::shared_ptr<SceneObject> m_targetObject = nullptr;
    };
}
