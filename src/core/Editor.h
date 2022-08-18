#pragma once

#include <memory>

#include <SDL.h>

#include <utils/PyxieHeaders.h>
#include <utils/Singleton.h>
#include <utils/Serialize.h>
#include <scene/SceneManager.h>
#include "core/Macros.h"

#include "core/scene/TargetObject.h"
#include "components/animation/AnimatorController.h"

using namespace pyxie;
using namespace ige::scene;

namespace ige::creator
{
    class Canvas;
    class ShortcutController;
    class Editor : public Singleton<Editor>
    {
    public:
        Editor();
        virtual ~Editor();

        virtual void initialize();
        virtual void update(float dt);
        virtual void render();
        static void handleEvent(const void* event);

        void resetLayout();
        void toggleFullScreen();

        bool buildWebGL();
        bool buildPC();
        bool buildAndroid();
        bool buildIOS();
        bool buildMacOS();

        bool openDocument();
        bool openAbout();

        bool createProject();
        bool openProject();

        bool createProjectInternal(const std::string& path);
        bool openProjectInternal(const std::string& path);

        bool createScene();
        bool loadScene(const std::string& path);
        void setCurrentScene(std::shared_ptr<Scene> scene);
        bool reloadResource(const std::string& path);

        bool unloadScene();
        bool saveScene();
        bool saveSceneAs();
        void refreshScene();

        bool openPrefab(const std::string& path);
        bool openPrefabById(const std::string& prefabId);
        bool reloadPrefab(uint64_t objectId);
        bool unpackPrefab(uint64_t objectId);
        bool closePrefab();
        bool savePrefab();
        bool savePrefabAs();

        bool openAnimator(const std::string& path);
        void createAnimator(const std::string& parent, const std::string& name);

        void createScript(const std::string& parent, const std::string& name);

        bool cloneObject();
        void copyObject();
        void pasteObject();
        bool deleteObject();

        void toggleBitmapFontCreator();
        void convertAssets();

        void registerApp(std::shared_ptr<Application> app) { m_app = app; }

        static std::shared_ptr<Application>& getApp() { return getInstance()->m_app; }
        static std::shared_ptr<Canvas>& getCanvas() { return getInstance()->m_canvas; }
        static std::shared_ptr<ShortcutController> getShortcut() {
            return hasInstance() ? getInstance()->m_shortcutController : nullptr;
        }

        //! Short-cut access to current scene
        static std::shared_ptr<Scene> getCurrentScene() { return SceneManager::getInstance()->getCurrentScene(); }

        //! Toggle local/global gizmo
        bool isLocalGizmo() { return m_bIsLocalGizmo; }
        void toggleLocalGizmo();

        //! Toggle perspective/orthor
        bool is3DCamera() { return m_bIs3DCamera; }
        void toggle3DCamera();

        //! Prefab save/load
        bool savePrefab(uint64_t objectId, const std::string& file);
        bool loadPrefab(uint64_t parentId, const std::string& file);

        //! Engine path
        const std::string& getEnginePath() const { return m_enginePath; }
        void setEnginePath(const std::string& path) { m_enginePath = path; }

        //! Project path
        const std::string& getProjectPath() const { return m_projectPath; }
        void setProjectPath(const std::string& path);

        //! Project filename
        const std::string& getProjectFileName() const { return m_projectFileName; }
        void setProjectFileName(const std::string& name) { m_projectFileName = name; }

        //! Add target
        void addTarget(std::shared_ptr<SceneObject> target, bool clear = false);

        //! Remove target
        void removeTarget(std::shared_ptr<SceneObject> target);

        //! Remove all target
        void clearTargets();

        //! Return the first selected object
        std::shared_ptr<SceneObject> getFirstTarget();

        //! Get targeted objects
        std::shared_ptr<TargetObject>& getTarget() { return m_target; }

        std::shared_ptr<AnimatorController> getCurrentAnimator();

        static ige::scene::Event<const std::shared_ptr<SceneObject>&>& getTargetAddedEvent() { return m_targetAddedEvent; }
        static ige::scene::Event<const std::shared_ptr<SceneObject>&>& getTargetRemovedEvent() { return m_targetRemovedEvent; }
        static ige::scene::Event<>& getTargetClearedEvent() { return m_targetClearedEvent; }

    protected:
        virtual void initImGUI();
        virtual bool handleEventImGUI(const SDL_Event* event);
        virtual void renderImGUI();
        virtual void setImGUIStyle();

    protected:
        std::shared_ptr<Canvas> m_canvas = nullptr;
        std::shared_ptr<Application> m_app = nullptr;
        std::shared_ptr<ShortcutController> m_shortcutController = nullptr;

        //! Path settings
        std::string m_enginePath;
        std::string m_projectPath;
        std::string m_projectFileName;

        //! Toggle local/global gizmo
        bool m_bIsLocalGizmo = true;

        //! Toggle 3D/2D camera
        bool m_bIs3DCamera = true;

        //! Selected objects json data
        json m_selectedJsons = json::array();

        //! Targeted events
        static ige::scene::Event<const std::shared_ptr<SceneObject>&> m_targetAddedEvent;
        static ige::scene::Event<const std::shared_ptr<SceneObject>&> m_targetRemovedEvent;
        static ige::scene::Event<> m_targetClearedEvent;

        //! Targeted object
        std::shared_ptr<TargetObject> m_target;
    };
        
    std::string GetEnginePath(const std::string& path);
    std::string GetRelativePath(const std::string& path);
}
