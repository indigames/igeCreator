#pragma once

#include "core/Panel.h"
#include "core/layout/Group.h"
#include "core/widgets/Image.h"
#include "core/scene/assets/AssetMeta.h"

#include <utils/PyxieHeaders.h>

namespace ige::creator
{
    class AnimatorPreview: public Panel
    {
    public:
        AnimatorPreview(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~AnimatorPreview();

        virtual void clear();
        void setModelPath(const std::string& path);
        Figure* getFigure() { return m_figure; }

        virtual void update(float dt);

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

    protected:
        Texture* m_rtTexture = nullptr;
        RenderTarget* m_fbo = nullptr;
        Camera* m_camera = nullptr;
        Showcase* m_showcase = nullptr;

        std::shared_ptr<Image> m_imageWidget = nullptr;
        std::string m_modelPath;
        Figure* m_figure = nullptr;
        bool m_bDirty = true;
    };
}
