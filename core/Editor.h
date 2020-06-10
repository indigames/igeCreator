#pragma once

#include <memory>

#include <SDL.h>

#include "utils/PyxieHeaders.h"
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

        void registerApp(std::shared_ptr<Application> app) { m_app = app; }

    protected:
        virtual void initImGUI();
        virtual bool handleEventImGUI(const SDL_Event* event);
        virtual void renderImGUI();

    protected:
        std::shared_ptr<Canvas> m_canvas = nullptr;
        std::shared_ptr<Application> m_app = nullptr;
    };
}
