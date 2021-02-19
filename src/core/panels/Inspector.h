#pragma once

#include "core/Panel.h"
#include "core/layout/Group.h"


#include "scene/SceneObject.h"
#include "components/physic/PhysicConstraint.h"

using namespace ige::scene;

namespace ige::creator
{
    class ComboBox;
    class InspectorEditor;

    class IgnoreTransformEventScope
    {
    public:
        IgnoreTransformEventScope(std::shared_ptr<SceneObject> obj, const std::function<void(SceneObject&)>& task)
            : m_object(obj), m_task(task)
        {
            m_object->getTransformChangedEvent().removeAllListeners();
        }

        ~IgnoreTransformEventScope()
        {
            m_object->getTransformChangedEvent().addListener(m_task);
            m_object = nullptr;
            m_task = nullptr;
        }

        std::shared_ptr<SceneObject> m_object;
        std::function<void(SceneObject&)> m_task;
    };

    class Inspector: public Panel
    {
    public:
        Inspector(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Inspector();
        virtual void clear();

        void setTargetObject(const std::shared_ptr<SceneObject>& obj);
        void updateMaterial(int index, const char* infoName, std::string txt);

        virtual void update(float dt) override;

        

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        //! Redraw
        void redraw() { m_bNeedRedraw = true;  }


    public:
        std::shared_ptr<InspectorEditor> getInspectorEditor() { return m_inspectorEditor; }
    protected:
        //! Inspected scene object
        std::shared_ptr<SceneObject> m_targetObject = nullptr;

        //! Groups of default components
        std::shared_ptr<Group> m_headerGroup = nullptr;
        std::shared_ptr<ComboBox> m_createCompCombo = nullptr;
        std::shared_ptr<Group> m_componentGroup = nullptr;

        //! Flags for redrawing component in main thread
        bool m_bNeedRedraw = false;
        std::shared_ptr<InspectorEditor> m_inspectorEditor = nullptr;
    };
}
