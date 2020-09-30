#include <imgui.h>

#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_opengl3.h>

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Inspector.h"
#include "core/panels/EditorScene.h"
#include "core/task/TaskManager.h"
#include "core/dialog/MsgBox.h"

#include <scene/SceneManager.h>
#include <scene/Scene.h>
#include <physic/PhysicManager.h>
using namespace ige::scene;

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
        initImGUI();

        // Create physic instance
        PhysicManager::getInstance()->initialize();

        m_canvas = std::make_shared<Canvas>();
        m_canvas->setDockable(true);
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
        if (objId == (uint64_t)-1 || !getSceneManager()->getCurrentScene())
        {
            setSelectedObject(nullptr);
            return;
        }
        auto obj = getSceneManager()->getCurrentScene()->findObjectById(objId);
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
        auto msgBox = MsgBox("About", "igeCreator \n Version: 0.0.1 \n ©2020 Indi Games", MsgBox::EBtnLayout::ok, MsgBox::EMsgType::info);
        while (!msgBox.ready(1000));
        return true;
    }
}
