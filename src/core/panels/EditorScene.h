#pragma once

#include <utils/PyxieHeaders.h>
using namespace pyxie;

#include "core/Panel.h"
#include "core/gizmo/Gizmo.h"

#include "components/gui/RectTransform.h"
#include "scene/Scene.h"
using namespace ige::scene;

namespace ige::creator
{
    class Image;

#define SYSTEM_KEYPRESSED(X)            (1 << ((X)-1))
#define SYSTEM_KEYCODE_ALT_VALUE        1
#define SYSTEM_KEYCODE_CTRL_VALUE       2
#define SYSTEM_KEYCODE_SHIFT_VALUE      3
#define SYSTEM_KEYCODE_ALT_MASK         SYSTEM_KEYPRESSED(SYSTEM_KEYCODE_ALT_VALUE)
#define SYSTEM_KEYCODE_CTRL_MASK        SYSTEM_KEYPRESSED(SYSTEM_KEYCODE_CTRL_VALUE)
#define SYSTEM_KEYCODE_SHIFT_MASK       SYSTEM_KEYPRESSED(SYSTEM_KEYCODE_SHIFT_VALUE)


#define RAD_TO_DEGREE                   57.2957795f
#define DEGREE_TO_RAD                   0.01745329f
#define DEG360_TO_RAD                   6.2831844f

    class EditorScene: public Panel
    {
    public:
        EditorScene(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~EditorScene();

        //! Set target object
        void setTargetObject(const std::shared_ptr<SceneObject>& obj);

        virtual void clear();
        virtual void update(float dt);
        virtual void refresh();

        bool isResizing();
        void set2DMode(bool is2D = true);

        std::shared_ptr<Gizmo>& getGizmo() { return m_gizmo; }

    protected:
        enum ViewTool
        {
            None                = -1,
            Pan                 = 0,
            Orbit               = 1,
            FPS                 = 2,
            Zoom                = 3,
            MultiSelectArea     = 4,
            MultiDeSelectArea   = 5,
        };


    protected:

        virtual void initialize() override;
        virtual void _drawImpl() override;

        void initDragDrop();

        //! Render bouding box
        void renderBoundingBox();

        //! Render camera frustum
        void renderCameraFrustum();

        //! Camera movement with mouse
        void updateCameraPosition(float touchX, float touchY);

        //! Object selection with touch/mouse
        void updateObjectSelection();

        //! Update keyboard
        void updateKeyboard();

        //! Update mouse & touch
        void updateTouch();

        void updateViewTool(int TouchID);

        //! reset showcase
        void resetShowcase();

        //! camera Look
        void lookAtObject(SceneObject* object);
        void handleCameraOrbit(float offsetX, float offsetY);
        void handleCameraPan(float offsetX, float offsetY);
        void handleCameraFPS(float offsetX, float offsetY);
        void handleCameraZoom(float offsetX, float offsetY);
        void handleCameraZoomFocus(float offsetX, float offsetY);

        //! camera helper functions
        float clampEulerAngle(float angle);
        Vec3 getForwardVector(Quat rot);
        void updateFocusPoint(bool resetView, bool resetFocus);
        float calcCameraViewDistance();
        float clampViewSize(float value);
        static float getPerspectiveCameraViewDistance(float size, float fov);
        static AABBox getRenderableAABBox(SceneObject* object);


    protected:
        //! Scene FBO
        std::shared_ptr<Image> m_imageWidget = nullptr;
        Texture* m_rtTexture = nullptr;
        RenderTarget* m_fbo = nullptr;

        //! Grids
        EditableFigure* m_grid2D = nullptr;
        EditableFigure* m_grid3D = nullptr; 

        //! Camera
        Camera* m_2dCamera = nullptr;
        Camera* m_3dCamera = nullptr;
        Camera* m_currCamera = nullptr;

        //! Showcase
        Showcase* m_currShowcase = nullptr;

        //! Gizmo
        std::shared_ptr<Gizmo> m_gizmo = nullptr;

        //! Cache current scene
        std::shared_ptr<Scene> m_currentScene = nullptr;

        //! Initialize states
        bool m_bIs2DMode = false;
        bool m_bIsInitialized = false;

        //keyboard helper
        unsigned short m_fnKeyPressed = 1;
        ViewTool m_viewTool = ViewTool::None;

        //Mouse Helper
        bool m_bIsFirstTouch = true;
        bool m_bIsDragging = false;
        //! use to determine which is click or drag
        float m_touchDelay = 0;

        //! Camera control
        int m_HandleCameraTouchId = -1;
        float m_cameraDragSpeed = 0.5f;
        float m_cameraRotationSpeed = 0.0033f;
        float m_lastMousePosX = 0.f;
        float m_lastMousePosY = 0.f;
        Vec3 m_cameraRotationEuler = {};

        Vec3 m_focusPosition = {};
        bool m_resetFocus = false;
        float m_cameraDistance = 0;

        const float k_defaultViewSize = 5;
        const float k_maxViewSize = 3.2e34f;
        const float k_minViewSize = 1e-5f;
        const float k_defaultFOV = 45;
        const float k_zoomFocusOffsetRate = 100.f;
        float m_viewSize = 0;
    };
}
