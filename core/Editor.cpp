#include <imgui.h>

#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_opengl3.h>

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Inspector.h"
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

        m_sceneManager = std::make_shared<SceneManager>();
        auto scene = m_sceneManager->createEmptyScene();
        m_sceneManager->setCurrentScene("EmptyScene");
        assert(scene == m_sceneManager->getCurrentScene());
    }

    void Editor::handleEvent(const void* event)
    {
        Editor::getInstance()->handleEventImGUI((const SDL_Event*)event);
    }

    void Editor::update(float dt)
    {
        // Update layouts
        m_canvas->update(dt);
    }

    void Editor::render()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame((SDL_Window*)m_app->getAppWindow());

        // Update inspector object
        auto inspector = getCanvas()->getPanelAs<Inspector>("Inspector");
        if (inspector && inspector->getTargetObject() != m_selectedObject)
        {
            inspector->setTargetObject(m_selectedObject);
        }

        // Render main canvas
        m_canvas->draw();

        // Update tasks
        TaskManager::getInstance()->update();

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
        m_selectedObject = getSceneManager()->getCurrentScene()->findObjectById(objId);
    }

}
