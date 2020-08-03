#pragma once

#include <memory>

#include <SDL.h>

#include <utils/PyxieHeaders.h>
#include <utils/Singleton.h>
#include <scene/SceneManager.h>

using namespace pyxie;
using namespace ige::scene;

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

        static std::shared_ptr<Application>& getApp() { return getInstance()->m_app; }
        static std::shared_ptr<Canvas>& getCanvas() { return getInstance()->m_canvas; }
        static std::unique_ptr<SceneManager>& getSceneManager() { return SceneManager::getInstance(); }

        //! Short-cut access to current scene
        static std::shared_ptr<Scene> getCurrentScene() { return getSceneManager() ? getSceneManager()->getCurrentScene() : nullptr; }
        static void setCurrentScene(const std::shared_ptr<Scene>& scene) { if (getSceneManager()) getSceneManager()->setCurrentScene(scene); }

        //! Set current selected object by its id
        static void setSelectedObject(uint64_t objId);
        static void setSelectedObject(const std::shared_ptr<SceneObject>& obj);
        static std::shared_ptr<SceneObject>& getSelectedObject();

    protected:
        virtual void initImGUI();
        virtual bool handleEventImGUI(const SDL_Event* event);
        virtual void renderImGUI();

    protected:
        std::shared_ptr<Canvas> m_canvas = nullptr;
        std::shared_ptr<Application> m_app = nullptr;
        std::shared_ptr<SceneObject> m_selectedObject = nullptr;
    };
}
