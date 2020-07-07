#pragma once

#include <scene/SceneObject.h>
using namespace ige::scene;

#include "core/Panel.h"
#include "core/widgets/TreeNode.h"

namespace ige::creator
{
    class Hierarchy: public Panel
    {
    public:
        Hierarchy(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Hierarchy();

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void drawWidgets() override;

        void onSceneObjectCreated(SceneObject& sceneObject);
        void onSceneObjectDeleted(SceneObject& sceneObject);

        void onSceneObjectAttached(SceneObject& sceneObject);
        void onSceneObjectDetached(SceneObject& sceneObject);

        void onSceneObjectChangedName(SceneObject& sceneObject);
        void onSceneObjectSelected(SceneObject& sceneObject);

        std::shared_ptr<TreeNode> m_sceneRoot;
        std::unordered_map<uint64_t, std::shared_ptr<TreeNode>> m_objectNodeMap;
        uint64_t m_selectedNodeId = 0xffffffff;
    };
}
