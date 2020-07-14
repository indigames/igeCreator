#pragma once

#include <memory>

#include <SDL.h>

#include <utils/PyxieHeaders.h>
#include <utils/Singleton.h>
#include <scene/SceneManager.h>

#include "core/Canvas.h"

using namespace pyxie;
using namespace ige::scene;

namespace ige::creator
{
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

        static std::shared_ptr<Application>& getApp() { return getInstance()->m_app; }
        static std::shared_ptr<Canvas>& getCanvas() { return getInstance()->m_canvas; }
        static SceneManager* getSceneManager() { return getInstance()->m_sceneManager; }

        //! Set current selected object by its id
        static void setSelectedObject(uint64_t objId);

    protected:
        virtual void initImGUI();
        virtual bool handleEventImGUI(const SDL_Event* event);
        virtual void renderImGUI();

    protected:
        std::shared_ptr<Canvas> m_canvas = nullptr;
        std::shared_ptr<Application> m_app = nullptr;
        SceneManager* m_sceneManager = nullptr;
    };
}
