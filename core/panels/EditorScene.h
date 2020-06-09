#pragma once

#include "core/Panel.h"

#include <pyxie.h>
#include <pyxieCamera.h>
#include <pyxieShowcase.h>
#include <pyxieEnvironmentSet.h>
#include <pyxieRenderContext.h>
#include <pyxieRenderTarget.h>
#include <pyxieFigure.h>
#include <pyxieEditableFigure.h>
#include <pyxieResourceCreator.h>
#include <pyxieResourceManager.h>

using Camera = pyxie::pyxieCamera;
using Showcase = pyxie::pyxieShowcase;
using Environment = pyxie::pyxieEnvironmentSet;
using RenderContext = pyxie::pyxieRenderContext;
using Figure = pyxie::pyxieFigure;
using EditableFigure = pyxie::pyxieEditableFigure;
using RenderTarget = pyxie::pyxieRenderTarget;
using ResourceCreator = pyxie::pyxieResourceCreator;
using ResourceManager = pyxie::pyxieResourceManager;


namespace ige::creator
{
    class Image;

    class EditorScene: public Panel
    {
    public:
        EditorScene(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~EditorScene();

        virtual void clear();

        void set2DMode(bool _2d) { m_bIs2DMode = _2d; }

        Camera* getCamera() const { return m_editorCamera; }
        Showcase* getShowcase() const { return m_editorShowcase; }
        Environment* getEnvironment() const { return m_editorEnvironment; }

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;
        EditableFigure* createGrid(const pyxie::Vec2& size, uint32_t textureId = 0xffffffff);
        

        Camera* m_editorCamera;
        Showcase* m_editorShowcase;
        Environment* m_editorEnvironment;
        RenderTarget* m_fbo;

        bool m_bIs2DMode = false;
        std::shared_ptr<Image> m_image = nullptr;
    };
}
