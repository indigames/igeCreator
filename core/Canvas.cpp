#include <imgui.h>
#include <imgui_internal.h>

#include "core/Canvas.h"
#include "core/Panel.h"
#include "core/menu/MenuBar.h"
#include "core/panels/Hierarchy.h"

namespace ige::creator
{
    Canvas::Canvas()
    {
        m_menuBar = std::make_shared<MenuBar>("Menu");

        Panel::Settings settings;
        settings.closable = true;
        settings.collapsable = true;
        settings.dockable = true;

        createPanel<Hierarchy>("Scene", settings);
        createPanel<Hierarchy>("Inspector", settings);
        createPanel<Hierarchy>("Hierarchy", settings);
    }


    Canvas::~Canvas()
    {
        m_menuBar = nullptr;
        m_panels.clear();
    }

    void Canvas::draw()
    {
        ImGui::NewFrame();

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

        ImGui::Begin("Workspace", nullptr, flags);
        m_menuBar->draw();        

        if (!m_panels.empty())
        {
            if (isDockable())
            {
                if (ImGui::DockBuilderGetNode(ImGui::GetID("EditorDockspace")) == NULL)
                {
                    ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");
                    ImGuiViewport* viewport = ImGui::GetMainViewport();
                    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
                    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
                    ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                    ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
                    ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
                    ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
                    ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);
                    ImGuiID dock_id_right_bottom = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.22f, NULL, &dock_id_right);

                    ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
                    ImGui::DockBuilderDockWindow("Scene", dock_main_id);
                    ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                    ImGui::DockBuilderDockWindow("Assets##ChooseFileDlgKey", dock_id_bottom);
                    ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
                    ImGui::DockBuilderDockWindow("AssetViewer", dock_id_right_bottom);
                    ImGui::DockBuilderFinish(dockspace_id);
                }

                ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");
                ImVec2 displaySize = ImGui::GetIO().DisplaySize;
                ImGui::DockSpace(dockspace_id, displaySize, ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);
                ImGui::SetWindowPos({ 0.f, 0.f });                
                ImGui::SetWindowSize({ (float)displaySize.x, (float)displaySize.y });          
            }

            for (auto panel : m_panels)
                panel.second->draw();
        }

        ImGui::End();
        ImGui::Render();
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