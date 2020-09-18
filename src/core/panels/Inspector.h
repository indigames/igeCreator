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

        void setTargetObject(const std::shared_ptr<SceneObject>& obj);
        void updateMaterial(int index, const char* infoName, std::string txt);

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        //! Draw transform components
        void drawLocalTransformComponent();
        void drawWorldTransformComponent();

        //! Draw camera component
        void drawCameraComponent();

        //! Draw environment component
        void drawEnvironmentComponent();

        //! Draw figure component
        void drawFigureComponent();

        //! Draw sprite component
        void drawSpriteComponent();

        //! Draw script component
        void drawScriptComponent();

        //! Draw RectTransform component
        void drawRectTransform();

        //! Draw Canvas component
        void drawCanvas();

        //! Draw UIImage component
        void drawUIImage();

        //! Draw UIText component
        void drawUIText();

        //! Draw PhysicBase component
        void drawPhysicBase();

        //! Draw PhysicBox component
        void drawPhysicBox();

        //! Draw PhysicSphere component
        void drawPhysicSphere();

        //! Draw PhysicCapsule component
        void drawPhysicCapsule();

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
        std::shared_ptr<Group> m_uiImageGroup = nullptr;
        std::shared_ptr<Group> m_uiTextGroup = nullptr;
        std::shared_ptr<Group> m_physicGroup = nullptr;

        //! Flags for redrawing component in main thread
        bool m_bNeedRedraw = false;
    };
}
