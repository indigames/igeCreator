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
        void updateMaterial(int index, const char* infoName, std::string txt);

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        //! Draw components
        void drawLocalTransformComponent();
        void drawWorldTransformComponent();
        void drawCameraComponent();
        void drawEnvironmentComponent();
        void drawFigureComponent();
        void drawSpriteComponent();
        void drawScriptComponent();
        void drawRectTransform();
        void drawCanvas();

        //! Redraw
        void redraw() { m_bNeedRedraw = true;  }

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
        std::shared_ptr<Group> m_spriteCompGroup = nullptr;
        std::shared_ptr<Group> m_scriptCompGroup = nullptr;
        std::shared_ptr<Group> m_rectTransformGroup = nullptr;
        std::shared_ptr<Group> m_canvasGroup = nullptr;

        //! Flags for redrawing component in main thread
        bool m_bNeedRedraw = false;
    };
}
