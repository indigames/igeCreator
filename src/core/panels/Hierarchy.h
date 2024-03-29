#pragma once

#include <scene/Scene.h>
#include <scene/SceneObject.h>
using namespace ige::scene;

#include "core/Panel.h"
#include "core/widgets/TreeNode.h"
#include "core/menu/ContextMenu.h"
#include "core/layout/Group.h"
#include "core/task/Timer.h"

namespace ige::creator
{
    class Hierarchy: public Panel
    {
    public:
        Hierarchy(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Hierarchy();

        virtual void clear();
        virtual void initialize() override;

        //! Highlight node
        void setNodeHighlight(uint64_t nodeId, bool highlight = true);

        //! Rebuild hierarchy from scene
        bool rebuildHierarchy();

    protected:
        virtual void drawWidgets() override;
        void addCreationContextMenu(SceneObject& sceneObject, std::shared_ptr<ContextMenu> ctxMenu);

        //! Object created/deleted
        void onSceneObjectCreated(SceneObject& sceneObject);
        void onSceneObjectDeleted(SceneObject& sceneObject);

        //! Object attached/detached to its parent
        void onSceneObjectAttached(SceneObject& sceneObject);
        void onSceneObjectDetached(SceneObject& sceneObject);

        //! Object selected
        void onSceneObjectSelected(SceneObject& sceneObject);

        //! Object activated
        void onSceneObjectActivated(SceneObject& sceneObject);

        //! Object changed name
        void onSceneObjectChangedName(SceneObject& sceneObject);

        //! Target object
        void onTargetAdded(const std::shared_ptr<SceneObject>& object);
        void onTargetRemoved(const std::shared_ptr<SceneObject>& object);
        void onTargetCleared();

        //! Tree node objects
        std::map<uint64_t, std::shared_ptr<TreeNode>> m_objectNodeMap;

        //! Group layout to add WindowContextMenu
        std::shared_ptr<Group> m_groupLayout = nullptr;;

        //! Use for catch click
        const float k_nodeDefaultHeight = 17;

        //! Highlight timer
        Timer m_highlightTimer;

        //! Skip deselect once contex menu item enqueue task
        bool m_bSkipDeselect = false;
    };
}
