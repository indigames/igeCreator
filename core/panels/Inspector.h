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

        void drawLocalTransformComponent();
        void drawWorldTransformComponent();
        void drawCameraComponent();
        void drawEnvironmentComponent();
        void drawFigureComponent();
        void drawEditableFigureComponent();

        //! Inspected scene object
        std::shared_ptr<SceneObject> m_targetObject = nullptr;

        //! Groups of default components
        std::shared_ptr<Group> m_headerGroup = nullptr;
        std::shared_ptr<ComboBox> m_createCompCombo = nullptr;
        std::shared_ptr<Group> m_componentGroup = nullptr;
        std::shared_ptr<Group> m_localTransformGroup = nullptr;
        std::shared_ptr<Group> m_worldTransformGroup = nullptr;
        std::shared_ptr<Group> m_cameraCompGroup = nullptr;
        std::shared_ptr<Group> m_cameraLockTargetGroup = nullptr;
        std::shared_ptr<Group> m_environmentCompGroup = nullptr;
        std::shared_ptr<Group> m_figureCompGroup = nullptr;
        std::shared_ptr<Group> m_editableFigureCompGroup = nullptr;
    };
}
