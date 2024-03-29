#pragma once

#include <utils/PyxieHeaders.h>

#include "core/Macros.h"
#include "core/Panel.h"
#include "core/gizmo/Gizmo.h"
#include "core/shortcut/IShortcut.h"

#include <components/gui/RectTransform.h>
#include <scene/Scene.h>

USING_NS_PYXIE
USING_NS_IGE_CREATOR
NS_IGE_BEGIN

class Image;

class EditorScene : public Panel, IShortcut
{
public:
    EditorScene(const std::string& name = "", const Panel::Settings& settings = {});
    virtual ~EditorScene();

    virtual void clear();
    virtual void update(float dt) override;

    void registerShortcut() override;
    void unregisterShortcut() override;

    bool isResizing();
    void set2DMode(bool is2D = true);
    bool is2DMode() const;

    std::shared_ptr<Gizmo>& getGizmo() { return m_gizmo; }

    void lookSelectedObject();
    void undo();
    void redo();

protected:
    enum class ViewTool
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

    void initDragDrop();

    //! Render bouding box
    void renderBoundingBoxes();
    void renderBoundingBox(SceneObject* object);

    //! Render camera frustum
    void renderCameraFrustum();

    //! Render object select rect
    void renderCameraSelect();

    //! Camera movement with mouse
    void updateCameraPosition(float touchX, float touchY);

    //! Object selection with touch/mouse
    void updateObjectSelection();

    //! Update keyboard
    void updateKeyboard();

    //! Update mouse & touch
    void updateTouch();

    void updateViewTool(int TouchID);

    //! camera Look
    void lookAtObject(SceneObject* object);
    void handleCameraOrbit(const Vec2& offset);
    void handleCameraPan(const Vec2& offset);
    void handleCameraFPS(const Vec2& offset);
    void handleCameraScroll(const Vec2& offset);
    void handleCameraZoom(const Vec2& offset);
    void handleCameraSelect(const Vec2& offset);

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

    //! Gizmo
    std::shared_ptr<Gizmo> m_gizmo = nullptr;

    //keyboard helper
    unsigned short m_fnKeyPressed = 1;
    ViewTool m_viewTool = ViewTool::None;

    //Mouse Helper
    bool m_bIsFirstTouch = true;
    bool m_bIsDragging = false; //! check is mouse if drag or not
    bool m_bIsDraggingCam = false; //! Mark Camera is dragging
    //! use to determine which is click or drag
    float m_touchDelay = 0;

    //! Camera control
    int m_HandleCameraTouchId = -1;
    float m_cameraDragSpeed = 0.5f;
    float m_cameraRotationSpeed = 0.0033f;
    float m_firstMousePosX = 0.f;
    float m_firstMousePosY = 0.f;
    float m_lastMousePosX = 0.f;
    float m_lastMousePosY = 0.f;
    Vec3 m_cameraRotationEuler = {};

    Vec3 m_focusPosition = {};
    bool m_resetFocus = false;
    float m_cameraDistance = 0;
    bool m_cameraMoving = false;

    const float k_defaultViewSize = 5;
    const float k_maxViewSize = 3.2e34f;
    const float k_minViewSize = 1e-5f;
    const float k_defaultFOV = 45;
    const float k_zoomFocusOffsetRate = 100.f;
    float m_viewSize = 0;

    //! Camera 2D
    float m_currentCanvasHeight = 0;
    float m_canvasRatio = 0;
};
NS_IGE_END
