#pragma once

#include "core/Panel.h"
#include "core/layout/Group.h"

#include "scene/SceneObject.h"
using namespace ige::scene;

namespace ige::creator
{
    class ComboBox;
    class Inspector: public Panel
    {
    public:
        Inspector(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Inspector();

        virtual void clear();

        void setTargetObject(std::shared_ptr<SceneObject> obj);
        std::shared_ptr<SceneObject>& getTargetObject() { return m_targetObject; }

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        void drawLocalTransformComponent(const std::shared_ptr<Component>& component);
        void drawWorldTransformComponent(const std::shared_ptr<Component>& component);

        std::shared_ptr<SceneObject> m_targetObject = nullptr;
        std::shared_ptr<Group> m_headerGroup = nullptr;
        std::shared_ptr<Group> m_componentGroup = nullptr;
        std::shared_ptr<Group> m_localTransformGroup = nullptr;
        std::shared_ptr<Group> m_worldTransformGroup = nullptr;
        std::shared_ptr<ComboBox> m_createCompCombo = nullptr;
    };
}
