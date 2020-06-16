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
        std::shared_ptr<Application>& getApp() { return m_app; }
        std::shared_ptr<SceneManager>& getSceneManager() { return m_sceneManager; }
        std::shared_ptr<Canvas>& getCanvas() { return m_canvas; }

        //! Get current selected object
        std::shared_ptr<SceneObject>& getSelectedObject() { return m_selectedObject; }

        //! Set current selected object by its name
        void setSelectedObject(const std::string& objName);

    protected:
        virtual void initImGUI();
        virtual bool handleEventImGUI(const SDL_Event* event);
        virtual void renderImGUI();

    protected:
        std::shared_ptr<Canvas> m_canvas = nullptr;
        std::shared_ptr<Application> m_app = nullptr;
        std::shared_ptr<SceneManager> m_sceneManager = nullptr;

        //! Selected object (in hierarchy & inspector)
        std::shared_ptr<SceneObject> m_selectedObject = nullptr;
    };
}
