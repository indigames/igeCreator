#include <imgui.h>

#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_opengl3.h>

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Inspector.h"
#include "core/panels/EditorScene.h"
#include "core/task/TaskManager.h"

#include <scene/SceneManager.h>
#include <scene/Scene.h>
using namespace ige::scene;

namespace ige::creator
{
    Editor::Editor()
    {}
    
    Editor::~Editor()
    {
        m_canvas = nullptr;
        m_sceneManager = nullptr;
        m_selectedObject = nullptr;
        SceneManager::destroy();
        m_app.reset();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }    

    void Editor::initialize()
    {
        initImGUI();

        m_canvas = std::make_shared<Canvas>();
        m_canvas->setDockable(true);

        m_sceneManager = SceneManager::getInstance();
        m_sceneManager->createEmptyScene();
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

        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)m_app->getAppWindow(), m_app->getAppContext());
        ImGui_ImplOpenGL3_Init("#version 130");
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
        auto obj = getSceneManager()->getCurrentScene()->findObjectById(objId);
        if (getInstance()->m_selectedObject != obj)
        {
            getInstance()->m_selectedObject = obj;
            getCanvas()->setTargetObject(obj);
        }        
    }
    
    std::shared_ptr<SceneObject>& Editor::getSelectedObject()
    {
        return getInstance()->m_selectedObject;
    }

}
