#include <imgui.h>

#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_opengl3.h>

#include "core/Version.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Inspector.h"
#include "core/panels/EditorScene.h"
#include "core/task/TaskManager.h"
#include "core/dialog/MsgBox.h"
#include "core/dialog/OpenFileDialog.h"
#include "core/dialog/SaveFileDialog.h"
#include "core/AutoReleasePool.h"
#include "core/shortcut/ShortcutController.h"

#include <scene/SceneManager.h>
#include <scene/Scene.h>
using namespace ige::scene;

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::creator
{
    Editor::Editor()
    {}

    Editor::~Editor()
    {
        m_selectedObject = nullptr;
        m_canvas = nullptr;

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
        m_projectPath = path;
        fs::current_path(m_projectPath);

        auto prjPath = fs::path(path);
        auto prjName = prjPath.stem().string();
        auto prjFile = prjPath.append(prjName + ".igeproj");

        if (!fs::exists(prjFile) && path.compare(getEnginePath()) != 0)
        {
            const auto copyOptions = fs::copy_options::overwrite_existing | fs::copy_options::recursive;
            fs::copy(GetEnginePath("project_template"), path, copyOptions);
        }

        SceneManager::getInstance()->setProjectPath(m_projectPath);
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
        m_canvas->update(dt);

        // Update shortcut
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
    }

    void Editor::resetLayout()
    {
        ImGui::LoadIniSettingsFromDisk(GetEnginePath("layout.ini").c_str());
    }

    void Editor::toggleFullScreen()
    {
        auto window = (SDL_Window*)m_app->getAppWindow();
        bool isFullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
        SDL_SetWindowFullscreen(window, isFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
    }

    bool Editor::handleEventImGUI(const SDL_Event* event)
    {
        return ImGui_ImplSDL2_ProcessEvent(event);
    }

    void Editor::renderImGUI()
    {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    //! Set current selected object by its Id
    void Editor::setSelectedObject(uint64_t objId)
    {
        if (objId == (uint64_t)-1 || !SceneManager::getInstance()->getCurrentScene())
        {
            setSelectedObject(nullptr);
            return;
        }
        auto obj = SceneManager::getInstance()->getCurrentScene()->findObjectById(objId);
        setSelectedObject(obj);
    }

    void Editor::setSelectedObject(std::shared_ptr<SceneObject> obj)
    {
        if (m_selectedObject != obj)
        {
            // Set previous object as not selected
            if(m_selectedObject) m_selectedObject->setSelected(false);

            // Notify the views
            if (obj) obj->setSelected(true);
            if (m_canvas) m_canvas->setTargetObject(obj);

            // Set current one as selected
            m_selectedObject = obj;
        }
    }

    std::shared_ptr<SceneObject>& Editor::getSelectedObject()
    {
        return m_selectedObject;
    }

    bool Editor::openProject(const std::string& path)
    {
        unloadScene();
        setProjectPath(path);

        auto prjPath = fs::path(path);
        auto prjName = prjPath.stem().string();
        auto prjFile = prjPath.append(prjName + ".igeproj");
        auto loaded = false;

        if (fs::exists(prjFile))
        {
            std::ifstream file(prjFile.string());
            if (file.is_open())
            {
                json settingsJson;
                file >> settingsJson;
                file.close();
                auto scenePath = fs::path(getProjectPath()).append(settingsJson.value("startScene", "scenes/main.scene"));
                if (fs::exists(scenePath))
                    loaded = loadScene(scenePath.string());
            }
        }
        
        if(!loaded)
        {
            auto scene = SceneManager::getInstance()->createScene();
            SceneManager::getInstance()->setCurrentScene(scene);
            SceneManager::getInstance()->saveScene(prjFile.parent_path().append("scenes").append("main.scene"));

            json settingsJson = json{
                {"startScene", "scenes/main.scene"},
            };

            std::ofstream file(prjFile.string());
            file << settingsJson;
            file.close();
            loaded = true;
        }

        // Refresh asset browser
        if (getCanvas() && getCanvas()->getAssetBrowser())
        {
            getCanvas()->getAssetBrowser()->setDirty();
        }


        return loaded;
    }

    bool Editor::loadScene(const std::string& path)
    {
        unloadScene();
        auto scenePath = fs::path(path);
        if (fs::exists(scenePath))
        {
            auto scene = SceneManager::getInstance()->loadScene(path);
            setCurrentScene(scene);
        }
        else
        {
            auto scene = SceneManager::getInstance()->createScene();
            scene->setPath(path);
            setCurrentScene(scene);
            saveScene();
        }
        return true;
    }

    bool Editor::unloadScene()
    {
        refreshScene();

        auto scene = SceneManager::getInstance()->getCurrentScene();
        if(scene) SceneManager::getInstance()->unloadScene(scene);

        if (getCanvas())
        {
            getCanvas()->getHierarchy()->clear();
            getCanvas()->getEditorScene()->clear();
            getCanvas()->getHierarchy()->initialize();
        }
        return true;
    }

    bool Editor::saveScene()
    {
        if (SceneManager::getInstance()->getCurrentScene() == nullptr)
            return false;

        if (SceneManager::getInstance()->getCurrentScene()->getPath().empty())
        {
            auto selectedFile = SaveFileDialog("Save Scene", ".", { "scene", "*.scene" }).result();
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
        if (SceneManager::getInstance()->getCurrentScene() == nullptr)
            return false;

        auto selectedFile = SaveFileDialog("Save Scene As", ".", { "scene", "*.scene" }).result();
        if (!selectedFile.empty())
        {
            SceneManager::getInstance()->saveScene(selectedFile);
        }
 
        return true;
    }

    void Editor::refreshScene() {
        setSelectedObject(-1);
        if(getCanvas()) getCanvas()->getEditorScene()->refresh();
    }

    bool Editor::convertAssets()
    {
        auto buildCmd = [](void*)
        {
            pyxie_printf("Converting assets...");
            system((std::string("python.exe ") + GetEnginePath("convert.py")).c_str());
            pyxie_printf("Converting assets DONE!");
            return 1;
        };

        auto buildThread = SDL_CreateThreadWithStackSize(buildCmd, "Build_Thread", 32 * 1024 * 1024, (void*)nullptr);
        SDL_DetachThread(buildThread);
        return true;
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

    bool Editor::buildIOS()
    {
        pyxie_printf("Build IOS: WIP...");
        return false;
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
        copyObject();
        pasteObject();
        return true;
    }

    bool Editor::deleteObject()
    {
        auto currentObject = Editor::getInstance()->getSelectedObject();
        if (currentObject)
        {
            if (currentObject->getParent())
                currentObject->getParent()->setSelected(true);
            else
                Editor::getInstance()->setSelectedObject(-1);
            Editor::getCurrentScene()->removeObjectById(currentObject->getId());
            return true;
        }
        return false;
    }

    void Editor::copyObject() 
    {
        auto currentObject = Editor::getInstance()->getSelectedObject();
        if (currentObject)
            currentObject->to_json(m_selectedJson);
        else
            m_selectedJson = json{};
    }

    void Editor::pasteObject() 
    {
        if (m_selectedJson.is_null())
            return;

        auto objName = m_selectedJson.value("name", "");
        auto newObject = Editor::getCurrentScene()->createObject(objName + "_cp");
        auto uuid = newObject->getUUID();
        newObject->from_json(m_selectedJson);
        newObject->setUUID(uuid);
        newObject->setName(objName + "_cp");
        newObject->setParent(getSelectedObject() && getSelectedObject()->getParent() ? getSelectedObject()->getParent() : nullptr);
        newObject->setSelected(true);
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
        if (SceneManager::getInstance()->getCurrentScene())
            return SceneManager::getInstance()->getCurrentScene()->savePrefab(objectId, file);
        return false;
    }

    bool Editor::loadPrefab(uint64_t parentId, const std::string& file)
    {
        if (SceneManager::getInstance()->getCurrentScene())
            return SceneManager::getInstance()->getCurrentScene()->loadPrefab(parentId, file);
        return false;
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
