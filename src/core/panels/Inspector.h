#pragma once
#include <functional>

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
        IgnoreTransformEventScope(std::shared_ptr<Component> comp, uint64_t& eventId, const std::function<void(SceneObject&)>& task);
        ~IgnoreTransformEventScope();

    protected:
        std::shared_ptr<Component> m_comp;
        std::function<void(SceneObject&)> m_task;
        uint64_t& m_eventId;
    };

    class Inspector: public Panel
    {
    public:
        Inspector(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Inspector();

        virtual void clear();
        virtual void initialize() override;

        virtual void update(float dt) override;
        void redraw() { m_bNeedRedraw = true; }

    protected:
        virtual void _drawImpl() override;

    public:
        std::shared_ptr<InspectorEditor> getInspectorEditor() { return m_inspectorEditor; }

    protected:
        //! Inspected scene object
        SceneObject* m_targetObject = nullptr;

        //! Groups of default components
        std::shared_ptr<Group> m_headerGroup = nullptr;
        std::shared_ptr<ComboBox> m_createCompCombo = nullptr;
        std::shared_ptr<Group> m_componentGroup = nullptr;

        //! Flags for redrawing component in main thread
        bool m_bNeedRedraw = false;
        std::shared_ptr<InspectorEditor> m_inspectorEditor = nullptr;
    };
}
