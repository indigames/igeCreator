#pragma once

#include <utils/PyxieHeaders.h>

#include "core/Panel.h"
#include "core/gizmo/Gizmo.h"

#include "components/RectTransform.h"
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

        void setTargetObject(std::shared_ptr<SceneObject> obj);

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        Camera* m_camera = nullptr;
        Showcase* m_showcase = nullptr;

        Texture* m_rtTexture = nullptr;
        RenderTarget* m_fbo = nullptr;

        Camera* m_guiCamera = nullptr;
        Showcase* m_guiShowcase = nullptr;

        Camera* m_currentCamera = nullptr;
        Showcase* m_currentShowcase = nullptr;

        EditableFigure* m_guiRect = nullptr;
        Vec2 m_guiRectSize;

        std::shared_ptr<Image> m_imageWidget = nullptr;
        std::shared_ptr<Gizmo> m_gizmo = nullptr;
        std::shared_ptr<SceneObject> m_targetObject = nullptr;
        std::shared_ptr<RectTransform> m_targetTransform = nullptr;

        bool m_bIs2DMode = false;
        bool m_bIsInitialized = false;
        bool m_bNeedResize = false;
    };
}
