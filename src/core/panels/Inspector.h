#pragma once

#include "core/Panel.h"
#include "core/layout/Group.h"

#include "scene/SceneObject.h"
#include "components/physic/PhysicConstraint.h"
using namespace ige::scene;

namespace ige::creator
{
    class ComboBox;

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

        //! Draw RectTransform anchor
        void drawRectTransformAnchor();

        //! Draw RectTransform component
        void drawRectTransform();

        //! Draw Canvas component
        void drawCanvas();

        //! Draw UIImage component
        void drawUIImage();

        //! Draw UIText component
        void drawUIText();

        //! Draw PhysicObject component
        void drawPhysicObject();

        //! Draw PhysicBox component
        void drawPhysicBox();

        //! Draw PhysicSphere component
        void drawPhysicSphere();

        //! Draw PhysicCapsule component
        void drawPhysicCapsule();

        //! Draw PhysicMesh component
        void drawPhysicMesh();

        //! Draw PhysicSoftBody component
        void drawPhysicSoftBody();

        //! Draw AudioSource component
        void drawAudioSource();

        //! Draw AudioListener component
        void drawAudioListener();

        //! Draw Ambient Light
        void drawAmbientLight();

        //! Draw Directional Light
        void drawDirectionalLight();

        //! Draw Point Light
        void drawPointLight();

        //! Draw Spot Light
        void drawSpotLight();

        //! Draw Physic Constraints
        void drawPhysicConstraints();

        //! Draw Physic Constraint
        void drawPhysicConstraint(const std::shared_ptr<PhysicConstraint>& constraint, std::shared_ptr<Group> group);

        //! Draw Fixed Constraint
        void drawFixedConstraint(const std::shared_ptr<PhysicConstraint>& constraint);

        //! Draw Hinge Constraint
        void drawHingeConstraint(const std::shared_ptr<PhysicConstraint>& constraint);

        //! Draw Slider Constraint
        void drawSliderConstraint(const std::shared_ptr<PhysicConstraint>& constraint);
        
        //! Draw Spring Constraint
        void drawSpringConstraint(const std::shared_ptr<PhysicConstraint>& constraint);

        //! Draw Dof6 Spring Constraint
        void drawDof6SpringConstraint(const std::shared_ptr<PhysicConstraint>& constraint);

        //! Draw Particle
        void drawParticle();

        //! Draw Navigable
        void drawNavigable();

        //! Draw NavMesh
        void drawNavMesh();

        //! Draw NavAgent
        void drawNavAgent();

        //! Redraw
        void redraw() { m_bNeedRedraw = true;  }

        //! onTransformChanged
        void onTransformChanged(SceneObject& sceneObject);

    protected:
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
        std::shared_ptr<Group> m_ambientLightGroup = nullptr;
        std::shared_ptr<Group> m_directionalLightGroup = nullptr;
        std::shared_ptr<Group> m_pointLightGroup = nullptr;
        std::shared_ptr<Group> m_spotLightGroup = nullptr;
        std::shared_ptr<Group> m_rectTransformGroup = nullptr;
        std::shared_ptr<Group> m_rectTransformAnchorGroup = nullptr;
        std::shared_ptr<Group> m_canvasGroup = nullptr;
        std::shared_ptr<Group> m_uiImageGroup = nullptr;
        std::shared_ptr<Group> m_uiTextGroup = nullptr;
        std::shared_ptr<Group> m_physicGroup = nullptr;
        std::shared_ptr<Group> m_audioSourceGroup = nullptr;
        std::shared_ptr<Group> m_audioListenerGroup = nullptr;
        std::shared_ptr<Group> m_constraintGroup = nullptr;
        std::shared_ptr<ComboBox> m_constraintCreateCombo = nullptr;
        std::shared_ptr<Group> m_particleGroup = nullptr;
        std::shared_ptr<Group> m_navigableGroup = nullptr;
        std::shared_ptr<Group> m_navMeshGroup = nullptr;
        std::shared_ptr<Group> m_navAgentGroup = nullptr;

        //! Flags for redrawing component in main thread
        bool m_bNeedRedraw = false;
    };
}
