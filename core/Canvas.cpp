#include <imgui.h>

#include "core/Canvas.h"
#include "core/Panel.h"

namespace ige::creator
{
    Canvas::Canvas() {}
    Canvas::~Canvas()
    {
        m_panels.clear();
    }

    void Canvas::draw()
    {
        if (!m_panels.empty())
        {
            // ImGui_ImplOpenGL3_NewFrame();
            // ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (isDockable())
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

                ImGui::Begin("#dockspace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
                ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
                ImGui::SetWindowPos({ 0.f, 0.f });
                ImVec2 displaySize = ImGui::GetIO().DisplaySize;
                ImGui::SetWindowSize({ (float)displaySize.x, (float)displaySize.y });
                ImGui::End();

                ImGui::PopStyleVar(3);
            }

            for (auto panel : m_panels)
                panel.second->draw();

            ImGui::Render();
        }
    }

    template<typename T, typename... Args>
    void Canvas::createPanel(const std::string& id, Args&&... args)
    {
        m_panels.emplace(id, std::make_shared<T>(std::forward<Args>(args)...));
    }

    template<typename T>
    std::shared_ptr<T> Canvas::getPanelAs(const std::string& id)
    {
        return m_panels[id];
    }

    void Canvas::removePanel(std::shared_ptr<Panel> panel)
    {
        auto found = std::find_if(m_panels.begin(), m_panels.end(), [&](auto itr)
        { 
            return itr.second == panel;
        });

        if (found != m_panels.end())
        {
            m_panels.erase(found);
        }
    }

    void Canvas::removePanelById(const std::string& id)
    {
        m_panels.erase(id);
    }
}