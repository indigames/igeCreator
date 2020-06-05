#pragma once

#include <memory>

#include <SDL.h>

#include <pyxieEditableFigure.h>
#include <pyxieShowcase.h>
#include <pyxieCamera.h>
#include <pyxieApplication.h>


#include "utils/Singleton.h"

using namespace pyxie;

namespace ige::creator
{
    class Canvas;
    class Editor : public Singleton<Editor>
    {
    public:
        Editor();
        virtual ~Editor();
        
        virtual void initialize();
        virtual void update(float dt);
        virtual void render();        
        static void handleEvent(const void* event);

        void registerApp(std::shared_ptr<pyxieApplication> app) { m_app = app; }

    protected:        
        virtual void createDeviceObjects();
	    virtual void refreshFontTexture();

        virtual void initImGUI();
        virtual bool handleEventImGUI(const SDL_Event* event);
        virtual void updateImGUI();
        virtual void renderImGUI();

    protected:
        std::shared_ptr<Canvas> m_canvas = nullptr;
        std::shared_ptr<pyxieApplication> m_app = nullptr;

        pyxieEditableFigure* m_editableFigure = nullptr;
        pyxieShowcase* m_showcase = nullptr;
        pyxieTexture* m_fontTexture = nullptr;
        pyxieCamera* m_camera = nullptr;
        std::map<uint32_t, Sampler> m_assetSamplers;
    };
}
