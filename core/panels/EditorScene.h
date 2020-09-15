#pragma once

#include <utils/PyxieHeaders.h>

#include "core/Panel.h"
#include "core/gizmo/Gizmo.h"

#include "components/gui/RectTransform.h"
#include "scene/Scene.h"
using namespace ige::scene;

namespace ige::creator
{
    class Image;

    class EditorScene: public Panel
    {
    public:
        EditorScene(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~EditorScene();

        virtual void clear();
        virtual void update(float dt);

        bool isResizing();
        void set2DMode(bool _2d) { m_bIs2DMode = _2d; }

        std::shared_ptr<Gizmo>& getGizmo() { return m_gizmo; }

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        void renderPhysicDebug();
        void updateCameraPosition();

        void onCameraChanged(CameraComponent* camera);

        Texture* m_rtTexture = nullptr;
        RenderTarget* m_fbo = nullptr;

        //! Grids
        EditableFigure* m_grid2D = nullptr;
        EditableFigure* m_grid3D = nullptr; 

        std::shared_ptr<Image> m_imageWidget = nullptr;
        std::shared_ptr<Gizmo> m_gizmo = nullptr;

        //! Cache current scene
        std::shared_ptr<Scene> m_currentScene = nullptr;
        CameraComponent* m_currentCamera = nullptr;

        //! Initialize states
        bool m_bIs2DMode = false;
        bool m_bIsInitialized = false;
        bool m_bNeedResize = false;

        //! Camera control
        bool m_bIsFirstTouch = true;
        float m_cameraDragSpeed = 0.5f;
        float m_cameraRotationSpeed = 0.005f;
        float m_lastMousePosX = 0.f;
        float m_lastMousePosY = 0.f;
        Vec3 m_cameraRotationEuler = {};
    };
}
