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

    class GameScene: public Panel
    {
    public:
        GameScene(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~GameScene();

        virtual void update(float dt);

        bool isPlaying() const { return m_bIsPlaying; }

        void play();
        void pause();
        void stop();

    protected:
        virtual void initialize() override;
        virtual void clear();

        virtual void _drawImpl() override;

    protected:
        //! Rendering FBO
        Texture* m_rtTexture = nullptr;
        RenderTarget* m_fbo = nullptr;
        std::shared_ptr<Image> m_imageWidget = nullptr;

        //! Cached states
        bool m_bInitialized = false;
        bool m_bIsPlaying = false;
        bool m_bIsPausing = false;
        bool m_bNeedResize = false;
        uint64_t m_lastObjectId = -1;
    };
}
