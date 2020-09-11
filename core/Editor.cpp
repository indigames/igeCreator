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
        PhysicManager::getInstance();

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

        // Update Physic
        PhysicManager::getInstance()->onUpdate(dt);

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

            m_selectedObject = obj;

            // Set current one as selected
            if (m_selectedObject) m_selectedObject->setSelected(true);

            if(m_canvas) m_canvas->setTargetObject(getSelectedObject());
        }
    }

    std::shared_ptr<SceneObject>& Editor::getSelectedObject()
    {
        return m_selectedObject;
    }
}
