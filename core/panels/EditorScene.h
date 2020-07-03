#pragma once

#include <utils/PyxieHeaders.h>

#include "core/Panel.h"
#include "core/gizmo/Gizmo.h"

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

        void set2DMode(bool _2d) { m_bIs2DMode = _2d; }

        std::shared_ptr<Gizmo>& getGizmo() { return m_gizmo; }

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        Camera* m_camera;
        Showcase* m_showcase;

        Texture* m_rtTexture;
        RenderTarget* m_fbo;

        std::shared_ptr<Image> m_imageWidget = nullptr;
        std::shared_ptr<Gizmo> m_gizmo = nullptr;

        bool m_bIs2DMode = false;
        bool m_bIsInitialized = false;
    };
}
