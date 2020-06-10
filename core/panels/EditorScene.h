#pragma once

#include "core/Panel.h"
#include "utils/PyxieHeaders.h"

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

        Camera* getCamera() const { return m_editorCamera; }
        Showcase* getShowcase() const { return m_editorShowcase; }
        Environment* getEnvironment() const { return m_editorEnvironment; }

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        Camera* m_editorCamera;
        Showcase* m_editorShowcase;
        Environment* m_editorEnvironment;
        Texture* m_rtTexture;
        RenderTarget* m_fbo;

        bool m_bIs2DMode = false;
        std::shared_ptr<Image> m_image = nullptr;
    };
}
