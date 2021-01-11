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
#include "core/dialog/SaveFileDialog.h"

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
        setSelectedObject(-1);
        m_canvas = nullptr;

        SceneManager::destroy();
        m_app = nullptr;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void Editor::initialize()
    {
        // Set engine path to the runtime path
        setEnginePath(fs::current_path().string());

        // Init ImGUI
        initImGUI();

        // Init Canvas
        m_canvas = std::make_shared<Canvas>();
        m_canvas->setDockable(true);

        // Set editor mode
        SceneManager::getInstance()->setIsEditor(true);
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

        // Load scene
        if (SceneManager::getInstance()->getCurrentScene() == nullptr)
        {
            // Load the start scene
            auto scene = SceneManager::getInstance()->loadScene(m_canvas->getProjectSetting()->getStartScene());

            // If no scene loaded, create empty scene
            if (scene == nullptr)
                scene = SceneManager::getInstance()->createScene();

            // Set current scene to new loaded/created scene
            Editor::setCurrentScene(scene);
        }
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
        io.Fonts->AddFontFromFileTTF("fonts/Manjari-Regular.ttf", 14.0f, &config);
        ImGui::StyleColorsDark();
    }

    void Editor::resetLayout()
    {
        ImGui::LoadIniSettingsFromDisk("layout.ini");
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

    bool Editor::createProject(const std::string& path)
    {
        if (!fs::is_empty(fs::path(path)))
        {
            auto btn = MsgBox("Warning", "Folder is not empty.\nDo you want to create project anyway?", MsgBox::EBtnLayout::yes_no, MsgBox::EMsgType::info).result();
            if (btn == MsgBox::EButton::no)
                return false;
        }
        
        return true;
    }

    bool Editor::createScene()
    {
        setSelectedObject(-1);
        SceneManager::getInstance()->unloadScene(SceneManager::getInstance()->getCurrentScene());
        setCurrentScene(SceneManager::getInstance()->createScene());
        return true;
    }

    bool Editor::loadScene(const std::string& path)
    {
        setSelectedObject(-1);
        SceneManager::getInstance()->unloadScene(SceneManager::getInstance()->getCurrentScene());
        getCanvas()->getHierarchy()->clear();
        getCanvas()->getEditorScene()->clear();
        getCanvas()->getHierarchy()->initialize();
        setCurrentScene(SceneManager::getInstance()->loadScene(path));
        return true;
    }

    bool Editor::saveScene()
    {
        if (SceneManager::getInstance()->getCurrentScene()->getPath().empty())
        {
            auto selectedFile = SaveFileDialog("Save", ".", { "scene", "*.scene" }).result();
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

    bool Editor::buildRom()
    {
        auto buildCmd = [](void*)
        {
            pyxie_printf("Building ROM...");
            system("cmd.exe /c tools\\build-rom.bat");
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
            system("cmd.exe /c tools\\build-game-pc.bat");
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
            system("cmd.exe /c tools\\build-game-android.bat");
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
}
