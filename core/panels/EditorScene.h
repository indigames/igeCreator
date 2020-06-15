#pragma once

#include <scene/SceneManager.h>
#include <utils/PyxieHeaders.h>

#include "core/Panel.h"

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

        void set2DMode(bool _2d) { m_bIs2DMode = _2d; }

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        Texture* m_rtTexture;
        RenderTarget* m_fbo;

        bool m_bIs2DMode = false;
        std::shared_ptr<Image> m_image = nullptr;

        std::shared_ptr<SceneManager> m_sceneManager = nullptr;
    };
}
