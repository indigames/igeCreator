#include "core/Version.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Inspector.h"
#include "core/panels/EditorScene.h"
#include "core/panels/Hierarchy.h"
#include "core/panels/ProjectSetting.h"
#include "core/panels/AssetBrowser.h"
#include "core/panels/AssetViewer.h"
#include "core/task/TaskManager.h"
#include "core/dialog/MsgBox.h"
#include "core/dialog/OpenFileDialog.h"
#include "core/dialog/SaveFileDialog.h"
#include "core/AutoReleasePool.h"
#include "core/shortcut/ShortcutController.h"
#include "core/panels/subpanels/BitmapFontCreator.h"
#include "core/scene/TargetObject.h"
#include "core/filesystem/FileSystem.h"

#include <scene/SceneManager.h>
#include <scene/Scene.h>
using namespace ige::scene;

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>
#define LAYOUT_CONFIG_INI "imgui.ini"

namespace ige::creator
{
    ige::scene::Event<const std::shared_ptr<SceneObject>&> Editor::m_targetAddedEvent;
    ige::scene::Event<const std::shared_ptr<SceneObject>&> Editor::m_targetRemovedEvent;
    ige::scene::Event<> Editor::m_targetClearedEvent;

    Editor::Editor()
    {}

    Editor::~Editor()
    {
        auto retPath = (fs::path(m_projectPath).append(LAYOUT_CONFIG_INI)).string();
        ImGui::SaveIniSettingsToDisk(retPath.c_str());

        m_target = nullptr;
        m_canvas = nullptr;
        m_selectedJsons.clear();

        if (m_shortcutController != nullptr) {
            m_shortcutController->release();
        }
        m_shortcutController = nullptr;

        PoolManager::destroyInstance();
        SceneManager::destroy();
        m_app = nullptr;        

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void Editor::setProjectPath(const std::string& path)
    { 
        if (m_projectPath.compare(path) != 0)
        {
            auto retPath = (fs::path(m_projectPath).append(LAYOUT_CONFIG_INI)).string();
            ImGui::SaveIniSettingsToDisk(retPath.c_str());
            unloadScene();
            m_projectPath = path;
            fs::current_path(m_projectPath);
            SceneManager::getInstance()->setProjectPath(m_projectPath);
            if (getCanvas() && getCanvas()->getAssetBrowser())
                getCanvas()->getAssetBrowser()->setDirty();
            retPath = (fs::path(m_projectPath).append(LAYOUT_CONFIG_INI)).string();
            ImGui::LoadIniSettingsFromDisk(retPath.c_str());
        }
    }

    void Editor::initialize()
    {
        // Set engine path to the runtime path
        setEnginePath(fs::current_path().string());

        // Pass engine path to Scene for debug purpose
        SceneManager::getInstance()->setEditorPath(getEnginePath());

        // Init ImGUI
        initImGUI();

        // Init Canvas
        m_canvas = std::make_shared<Canvas>();
        m_canvas->setDockable(true);

        // Set editor mode
        SceneManager::getInstance()->setIsEditor(true);

        //! Init Shortcut
        m_shortcutController = std::make_shared<ShortcutController>();
        m_shortcutController->retain();

        //! init default shortcut
        ShortcutDictionary::initShortcuts();
    }

    void Editor::handleEvent(const void* event)
    {
        Editor::getInstance()->handleEventImGUI((const SDL_Event*)event);
    }

    void Editor::update(float dt)
    {
        // Set ImGui IO DeltaTime
        ImGui::GetIO().DeltaTime = dt;

        // Update tasks
        TaskManager::getInstance()->update();

        // Update layouts
        if(m_canvas)
            m_canvas->update(dt);

        // Update shortcut
        if(m_shortcutController)
            m_shortcutController->update(dt);
    }

    void Editor::render()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame((SDL_Window*)m_app->getAppWindow());

        // Render main canvas
        m_canvas->draw();

        // Render ImGUI
        renderImGUI();
    }

    void Editor::initImGUI()
    {
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        setImGUIStyle();

        ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)m_app->getAppWindow(), m_app->getAppContext());
        ImGui_ImplOpenGL3_Init("#version 130");
    }

    void Editor::setImGUIStyle()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig config;
        io.Fonts->AddFontFromFileTTF(GetEnginePath("fonts/Manjari-Regular.ttf").c_str(), 14.0f, &config);
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = { 2.f, 2.f };
    }

    void Editor::resetLayout()
    {
        try
        {
            auto retPath = (fs::path(m_projectPath).append(LAYOUT_CONFIG_INI)).string();
            fs::copy(GetEnginePath("layout.ini"), retPath, fs::copy_options::overwrite_existing);
            ImGui::LoadIniSettingsFromDisk(retPath.c_str());
        }
        catch (std::exception& ex) {}
    }

    void Editor::toggleFullScreen()
    {
        auto window = (SDL_Window*)m_app->getAppWindow();
        bool isFullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
        SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    }

    void Editor::toggleBitmapFontCreator()
    {
        auto panel = getCanvas()->getPanelAs<BitmapFontCreator>("BitmapFontCreator");
        if (panel != nullptr)
        {
            panel->open();
        }
    }

    void Editor::convertAssets()
    {
        auto buildCmd = [](void* param)
        {
            pyxie_printf("Converting assets...");
            auto scriptPath = fs::path(Editor::getInstance()->getEnginePath()).append("tools").append("convert-textures.bat");
            auto projectDir = Editor::getInstance()->getProjectPath();
            auto cmd = (std::string("cmd.exe /c \"\"") + scriptPath.string() + "\" \"" + projectDir +"\"\"").c_str();
            system(cmd);
            pyxie_printf("Converting assets finished!");
            return 1;
        };
        auto buildThread = SDL_CreateThreadWithStackSize(buildCmd, "Build_Thread", 32 * 1024 * 1024, (void*)NULL);
        SDL_DetachThread(buildThread);
    }

    void Editor::toggleReloadSource()
    {
        auto scene = SceneManager::getInstance()->getCurrentScene();
        if (scene != nullptr) {
            auto root = scene->getRoot();
            if (root != nullptr) {
                root->reloadScripts();
            }
        }
    }

    bool Editor::handleEventImGUI(const SDL_Event* event)
    {
        return ImGui_ImplSDL2_ProcessEvent(event);
    }

    void Editor::renderImGUI()
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool Editor::createProject()
    {
        auto path = OpenFolderDialog("New Project").result();
        if (!path.empty())
        {
            TaskManager::getInstance()->addTask([path]() {
                Editor::getInstance()->createProjectInternal(path);
            });
        }
        return true;
    }

    bool Editor::createProjectInternal(const std::string& path)
    {
        auto prjPath = fs::path(path);
        auto prjName = prjPath.stem().string();
        auto prjFile = prjPath.append(prjName + ".igeproj");
        auto loaded = false;

        if (fs::exists(prjFile))
        {
            auto btn = MsgBox("Project Exist", "Do you want to overwrite?", MsgBox::EBtnLayout::ok_cancel, MsgBox::EMsgType::question).result();
            if (btn == MsgBox::EButton::cancel)
            {
                return false;
            }
        }

        if (path.compare(getEnginePath()) != 0)
        {
            try
            {
                fs::remove_all(path);
            }
            catch (std::exception& ex) {}

            const auto copyOptions = fs::copy_options::overwrite_existing | fs::copy_options::recursive;
            fs::copy(GetEnginePath("project_template"), path, copyOptions);
        }

        setProjectPath(path);
        createScene();
        SceneManager::getInstance()->saveScene(prjFile.parent_path().append("scenes").append("main.scene"));
        json settingsJson = json{
            {"startScene", "scenes/main.scene"},
        };
        std::ofstream file(prjFile.string());
        file << settingsJson;
        file.close();

        return true;
    }
    
    bool Editor::openProject()
    {
        auto path = OpenFolderDialog("Open Project", ".", OpenFileDialog::Option::force_path).result();
        if (!path.empty())
        {
            TaskManager::getInstance()->addTask([path]() {
                Editor::getInstance()->openProjectInternal(path);
            });
        }
        return true;
    }

    bool Editor::openProjectInternal(const std::string& path)
    {
        // Stop previous logging process if any
        pyxie_logg_stop();

        auto prjPath = fs::path(path);
        auto prjName = prjPath.stem().string();
        auto prjFile = prjPath.append(prjName + ".igeproj");
        auto loaded = false;

        if (fs::exists(prjFile))
        {
            setProjectPath(path);

            // Start new logging process
            auto logFilePath = fs::path(path).append("error.log");
            if (fs::exists(logFilePath)) {
                std::error_code ec;
                fs::remove(logFilePath, ec);
            }
            pyxie_logg_start();

            std::ifstream file(prjFile.string());
            if (file.is_open())
            {
                json settingsJson;
                file >> settingsJson;
                file.close();
                auto scenePath = fs::path(getProjectPath()).append(settingsJson.value("startScene", "scenes/main.scene"));
                if (fs::exists(scenePath))
                {
                    loaded = loadScene(scenePath.string());
                }
            }
        }
        else
        {
            auto btn = MsgBox("Project Does Not Exist", "Do you want to create?", MsgBox::EBtnLayout::ok_cancel, MsgBox::EMsgType::question).result();
            if (btn == MsgBox::EButton::ok)
            {
                auto fsPath = fs::path(path);
                fs::remove_all(fsPath);
                return createProjectInternal(path);
            }
        }

        return loaded;
    }

    bool Editor::reloadResource(const std::string& path) {
        auto& scene = SceneManager::getInstance()->getCurrentScene();
        if (!scene) return false;
        return scene->reloadResource(path);
    }

    bool Editor::createScene()
    {
        unloadScene();
        auto scene = SceneManager::getInstance()->createScene();
        m_target = std::make_shared<TargetObject>(scene.get());
        setCurrentScene(scene);
        return true;
    }

    void Editor::setCurrentScene(std::shared_ptr<Scene> scene)
    { 
        SceneManager::getInstance()->setCurrentScene(scene);

        if (getCanvas())
        {
            getCanvas()->getMenuBar()->initialize();
            getCanvas()->getProjectSetting()->initialize();
            getCanvas()->getHierarchy()->initialize();
            getCanvas()->getHierarchy()->rebuildHierarchy();
            getCanvas()->getEditorScene()->set2DMode(!Editor::getInstance()->is3DCamera());
        }
    }

    bool Editor::loadScene(const std::string& path)
    {
        unloadScene();
        auto scenePath = fs::path(path);
        if (fs::exists(scenePath))
        {
            auto scene = SceneManager::getInstance()->createScene();
            m_target = std::make_shared<TargetObject>(scene.get());

            auto success = SceneManager::getInstance()->loadScene(scene, path);
            if (success)
            {
                setCurrentScene(scene);
            }
            else
            {
                SceneManager::getInstance()->unloadScene(scene);
                scene = nullptr;
                m_target = nullptr;
            }
        }
        return true;
    }

    bool Editor::unloadScene()
    {
        refreshScene();
        clearTargets();
        m_target = nullptr;

        auto scene = Editor::getCurrentScene();
        if(scene) SceneManager::getInstance()->unloadScene(scene);
        scene = nullptr;
        return true;
    }

    bool Editor::saveScene()
    {
        if (Editor::getCurrentScene() == nullptr)
            return false;

        // Not allow saving while playing
        if (getCanvas() && (getCanvas()->getGameScene()->isPlaying() || getCanvas()->getGameScene()->isPausing()))
            return false;
        
        if (Editor::getCurrentScene()->getPath().empty())
        {
            auto selectedFile = SaveFileDialog("Save Scene", "scenes", { "scene", "*.scene" }).result();
            if (!selectedFile.empty())
            {
                SceneManager::getInstance()->saveScene(selectedFile);
            }
        }
        else
        {
            SceneManager::getInstance()->saveScene();
        }

        return true;
    }

    bool Editor::saveSceneAs()
    {
        if (Editor::getCurrentScene() == nullptr)
            return false;

        // Not allow saving while playing
        if (getCanvas() && (getCanvas()->getGameScene()->isPlaying() || getCanvas()->getGameScene()->isPausing()))
            return false;

        auto selectedFile = SaveFileDialog("Save Scene As", "scenes", { "scene", "*.scene" }).result();
        if (!selectedFile.empty())
        {
            SceneManager::getInstance()->saveScene(selectedFile);
        }
 
        return true;
    }

    void Editor::refreshScene() {
        if (getCanvas())
        {
            getCanvas()->getHierarchy()->clear();
            getCanvas()->getInspector()->clear();
        }
    }

    bool Editor::openPrefab(const std::string& path)
    {
        if (SceneManager::getInstance()->isPlaying()) 
            return false;

        auto fsPath = fs::path(path);
        if (!fs::exists(fsPath) || fsPath.extension().string() != ".prefab")
            return false;

        refreshScene();
        clearTargets();
        m_target = nullptr;

        auto scene = SceneManager::getInstance()->createSceneFromPrefab(path);
        if (scene)
        {
            m_target = std::make_shared<TargetObject>(scene.get());
            setCurrentScene(scene);
            return true;
        }        
        return false;
    }

    bool Editor::openPrefabById(const std::string& prefabId)
    {
        return openPrefab(SceneManager::getInstance()->getPrefabPath(prefabId));
    }

    bool Editor::reloadPrefab(uint64_t objectId)
    {
        auto sceneObject = getCurrentScene()->findObjectById(objectId);
        if (sceneObject != nullptr) {
            auto prefabRoot = sceneObject->getPrefabRoot();
            if (prefabRoot != nullptr) {
                auto prefabId = sceneObject->getPrefabRootId();
                auto parentObject = prefabRoot->getParent();
                getCurrentScene()->removeObject(prefabRoot);
                prefabRoot = nullptr;
                if (parentObject != nullptr) {
                    getCurrentScene()->loadPrefab(parentObject->getId(), SceneManager::getInstance()->getPrefabPath(prefabId));
                    if(getCanvas()) getCanvas()->getHierarchy()->rebuildHierarchy();
                }
                return true;
            }
        }
        return false;
    }

    bool Editor::unpackPrefab(uint64_t objectId)
    {
        auto sceneObject = getCurrentScene()->findObjectById(objectId);
        if (sceneObject != nullptr) {
            auto prefabRoot = sceneObject->getPrefabRoot();
            if (prefabRoot != nullptr) {
                prefabRoot->unpackPrefab();
                if(getCanvas()) getCanvas()->getHierarchy()->rebuildHierarchy();
                return true;
            }
        }
        return false;
    }

    bool Editor::closePrefab()
    {
        auto scene = SceneManager::getInstance()->getCurrentScene();
        if (scene)
        {
            bool saved = false;
            auto prefabId = scene->getPrefabId();
            auto btn = MsgBox("Save prefab", "Do you want to save changes?", MsgBox::EBtnLayout::yes_no, MsgBox::EMsgType::question).result();
            if (btn == MsgBox::EButton::yes) {
                saved = savePrefab();
            }
            unloadScene();
            scene = SceneManager::getInstance()->getScenes().back();
            if (scene != nullptr) {
                m_target = std::make_shared<TargetObject>(scene.get());
                if (saved) {
                    auto btn = MsgBox("Reload prefab", "Do you want to reload all the nodes with the changed prefab?", MsgBox::EBtnLayout::yes_no, MsgBox::EMsgType::question).result();
                    if (btn == MsgBox::EButton::yes) {
                        scene->reloadPrefabs(prefabId);
                    }
                }
                setCurrentScene(scene);
            }
            return true;
        }

        scene = SceneManager::getInstance()->getScenes().back();
        m_target = std::make_shared<TargetObject>(scene.get());
        setCurrentScene(scene);
        return false;
    }

    bool Editor::savePrefab()
    {
        auto scene = SceneManager::getInstance()->getCurrentScene();
        if (scene && scene->isPrefab()) {
            auto prefabRootId = scene->getObjects()[0]->getId();            
            auto path = scene->getPath();;
            return savePrefab(prefabRootId, path);
        }
        return false;
    }

    bool Editor::savePrefabAs()
    {
        auto scene = SceneManager::getInstance()->getCurrentScene();
        if (scene && scene->isPrefab()) {
            auto prefabRootId = scene->getObjects()[0]->getId();
            auto selectedFile = SaveFileDialog("Save Prefab As", "prefab", { "prefab", "*.prefab" }).result();
            if (!selectedFile.empty()) {
                return savePrefab(prefabRootId, selectedFile);
            }
        }
        return false;
    }

    bool Editor::buildRom()
    {
        auto buildCmd = [](void*)
        {
            pyxie_printf("Building ROM...");
            auto scriptPath = fs::path(Editor::getInstance()->getEnginePath()).append("tools").append("build-rom.bat");
            auto projectDir = Editor::getInstance()->getProjectPath();
            system((std::string("cmd.exe /c ") + scriptPath.string() + " " + projectDir).c_str());
            pyxie_printf("Building ROM: DONE!");
            return 1;
        };

        auto buildThread = SDL_CreateThreadWithStackSize(buildCmd, "Build_Thread", 32 * 1024 * 1024, (void*)nullptr);
        SDL_DetachThread(buildThread);
        return true;
    }

    bool Editor::buildPC()
    {
        auto buildCmd = [](void*)
        {
            pyxie_printf("Building Windows Desktop...");
            auto scriptPath = fs::path(Editor::getInstance()->getEnginePath()).append("tools").append("build-windows.bat");
            auto projectDir = Editor::getInstance()->getProjectPath();
            system((std::string("cmd.exe /c ") + scriptPath.string() + " " + projectDir).c_str());
            pyxie_printf("Building Windows Desktop: DONE!");
            return 1;
        };

        auto buildThread = SDL_CreateThreadWithStackSize(buildCmd, "Build_Thread", 32 * 1024 * 1024, (void*)nullptr);
        SDL_DetachThread(buildThread);
        return true;
    }

    bool Editor::buildAndroid()
    {
        auto buildCmd = [](void*)
        {
            pyxie_printf("Building Android...");
            auto scriptPath = fs::path(Editor::getInstance()->getEnginePath()).append("tools").append("build-android.bat");
            auto projectDir = Editor::getInstance()->getProjectPath();
            system((std::string("cmd.exe /c ") + scriptPath.string() + " " + projectDir).c_str());
            pyxie_printf("Building Android: DONE!");
            return 1;
        };

        auto buildThread = SDL_CreateThreadWithStackSize(buildCmd, "Build_Thread", 32 * 1024 * 1024, (void*)nullptr);
        SDL_DetachThread(buildThread);
        return true;
    }

    bool Editor::openDocument()
    {
#ifdef WIN32
        ShellExecute(0, 0, "https://indigames.net", 0, 0, SW_SHOW);
#else
        system("open https://indigames.net");
#endif
        return true;
    }

    bool Editor::openAbout()
    {
        auto msgBox = MsgBox("About", "igeCreator \n Version: " + std::string(VERSION) + "\n Indi Games © 2020", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::info);
        while (!msgBox.ready(1000));
        return true;
    }

    bool Editor::cloneObject()
    {
        json clonedJson;
        auto targets = Editor::getInstance()->getTarget()->getAllTargets();
        if (targets.size() > 0 && !targets[0].expired())
        {
            for (auto& target : targets)
            {
                if (!target.expired())
                {
                    json jTarget;
                    target.lock()->to_json(jTarget);
                    if (!jTarget.is_null())
                        clonedJson.push_back(jTarget);
                }
            }
            for (auto& jTarget : clonedJson)
            {
                auto objName = jTarget.value("name", "");
                auto isGui = jTarget.value("gui", false);
                auto newObject = Editor::getCurrentScene()->createObject(objName + "_cp", nullptr, isGui);
                auto uuid = newObject->getUUID();
                jTarget["selected"] = false; // This is new node, should not auto selected
                newObject->from_json(jTarget);
                newObject->setUUID(uuid);
                newObject->setName(objName + "_cp");
                newObject->setParent(targets[0].lock()->getParent());
            }
            return true;
        }        
        return false;
    }

    bool Editor::deleteObject()
    {
        if (!Editor::getInstance()->getTarget()) return false;

        if (getCanvas())
            getCanvas()->getInspector()->clear();

        for (auto& target : Editor::getInstance()->getTarget()->getAllTargets()) {
            if (!target.expired()) {
                removeTarget(target.lock());
                Editor::getCurrentScene()->removeObject(target.lock());
            }
        }

        clearTargets();
        return true;
    }

    void Editor::copyObject() 
    {
        m_selectedJsons.clear();
        auto targets = Editor::getInstance()->getTarget()->getAllTargets();
        for (const auto& target : targets)
        {
            json jTarget;
            target.lock()->to_json(jTarget);
            if(!jTarget.is_null())
                m_selectedJsons.push_back(jTarget);
        }
    }

    void Editor::pasteObject()
    {
        if (m_selectedJsons.empty()) return;

        for (const auto& jTarget : m_selectedJsons)
        {
            auto objName = jTarget.value("name", "");
            auto newObject = Editor::getCurrentScene()->createObject(objName + "_cp");
            auto uuid = newObject->getUUID();
            newObject->from_json(jTarget);
            newObject->setUUID(uuid);
            newObject->setName(objName + "_cp");
            auto parent = Editor::getInstance()->getFirstTarget()->getSharedPtr();
            newObject->setParent(parent);
        }
    }

    void Editor::toggleLocalGizmo()
    {
        m_bIsLocalGizmo = !m_bIsLocalGizmo;
        if (m_canvas && m_canvas->getEditorScene() && m_canvas->getEditorScene()->getGizmo())
        {
            m_canvas->getEditorScene()->getGizmo()->setMode(m_bIsLocalGizmo ? gizmo::MODE::LOCAL : gizmo::MODE::WORLD);
        }
    }

    void Editor::toggle3DCamera()
    {
        m_bIs3DCamera = !m_bIs3DCamera;
        if (m_canvas && m_canvas->getEditorScene())
        {
            m_canvas->getEditorScene()->set2DMode(!m_bIs3DCamera);
        }
    }

    //! Prefab save/load
    bool Editor::savePrefab(uint64_t objectId, const std::string& file)
    {
        if (!Editor::getCurrentScene()) return false;
        auto object = Editor::getCurrentScene()->findObjectById(objectId);
        if (object != nullptr)
        {
            auto fsPath = file.empty() ? fs::path(object->getName() + ".prefab") : fs::path(file);
            if (fs::is_directory(fsPath)) fsPath = fsPath.append(object->getName() + ".prefab");
            auto ext = fsPath.extension();
            if (ext.string() != ".prefab") 
                fsPath = fsPath.replace_extension(".prefab");
            if (!fs::exists(fsPath)) {
                return Editor::getCurrentScene()->savePrefab(objectId, fsPath.string());
            }
            else
            {
                auto btn = MsgBox("Save prefab", "Prefab file exists. Do you want to overwrite?", MsgBox::EBtnLayout::yes_no, MsgBox::EMsgType::question).result();
                if (btn == MsgBox::EButton::yes) {
                    return Editor::getCurrentScene()->savePrefab(objectId, fsPath.string());
                }
            }
        }
        return false;
    }

    bool Editor::loadPrefab(uint64_t parentId, const std::string& file)
    {
        if (Editor::getCurrentScene())
            return Editor::getCurrentScene()->loadPrefab(parentId, file) != nullptr;
        return false;
    }

    //! Add target
    void Editor::addTarget(std::shared_ptr<SceneObject> target, bool clear)
    {
        if (m_target)
        {
            if (clear)
                clearTargets();

            if (target)
            {
                m_target->add(target);
                getTargetAddedEvent().invoke(target);
            }
        }        
    }

    //! Remove target
    void Editor::removeTarget(std::shared_ptr<SceneObject> target)
    {
        if (m_target)
        {            
            m_target->remove(target);
            getTargetRemovedEvent().invoke(target);
        }
    }

    //! Remove all target
    void Editor::clearTargets()
    {
        if (m_target)
        {
            m_target->clear();
            getTargetClearedEvent().invoke();
        }
    }

    //! Return the first selected object
    std::shared_ptr<SceneObject> Editor::getFirstTarget()
    {
        return m_target ? m_target->getFirstTarget() : nullptr;
    }

    const auto script_template =
        "from igeScene import Script\n\
\n\
class %s(Script):\n\
    def __init__(self, owner):\n\
        super().__init__(owner)\n\
        print(f'Hello {self.owner.name}')\n\
    \n\
    def onAwake(self):\n\
        print('onAwake() called!')\n\
    \n\
    def onStart(self):\n\
        print('onStart() called!')\n\
    \n\
    def onUpdate(self, dt):\n\
        pass\n\
    \n\
    def onClick(self):\n\
        print(f'onClick(): {self.owner.name}!')\n\
    \n\
    def onDestroy(self):\n\
        print('onDestroy called!')\n\
    \n";

    std::string CreateScript(const std::string& name)
    {
        char script[512] = { 0 };
        sprintf(script, script_template, name.c_str());

        auto fileName = name;
        std::transform(fileName.begin(), fileName.end(), fileName.begin(),
            [](unsigned char c) { return std::tolower(c); });
        fs::path path{ "scripts/" + fileName + ".py" };
        std::ofstream ofs(path);
        ofs << script;
        ofs.close();
        return fileName;
    }

    std::string GetEnginePath(const std::string& path)
    {
        if (Editor::getInstance()->getEnginePath().compare(Editor::getInstance()->getProjectPath()) == 0)
            return path;
        auto retPath = (fs::path(Editor::getInstance()->getEnginePath()).append(path)).string();
        std::replace(retPath.begin(), retPath.end(), '\\', '/');
        return retPath;
    }
}
