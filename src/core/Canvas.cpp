#include <imgui.h>
#include <imgui_internal.h>

#include "core/Canvas.h"
#include "core/Panel.h"
#include "core/menu/MenuBar.h"
#include "core/toolbar/ToolBar.h"
#include "core/panels/EditorScene.h"
#include "core/panels/GameScene.h"
#include "core/panels/Hierarchy.h"
#include "core/panels/Inspector.h"
#include "core/panels/Console.h"
#include "core/panels/AssetViewer.h"
#include "core/panels/AssetBrowser.h"

namespace ige::creator
{
    Canvas::Canvas()
    {
        m_menuBar = std::make_shared<MenuBar>("Menu");
        m_toolBar = std::make_shared<ToolBar>("ToolBar");
        
        Panel::Settings settings;
        settings.collapsable = true;
        settings.dockable = true;
        settings.movable = false;

        m_inspector = createPanel<Inspector>("Inspector", settings);
        m_hierarchy = createPanel<Hierarchy>("Hierarchy", settings);
        m_console = createPanel<Console>("Console", settings);
        createPanel<AssetBrowser>("AssetBrowser", settings);
        m_editorScene = createPanel<EditorScene>("Scene", settings);
        m_gameScene = createPanel<GameScene>("Game", settings);
        m_gameScene->close();
    }

    Canvas::~Canvas()
    {
        m_menuBar = nullptr;
        m_toolBar = nullptr;
        m_inspector = nullptr;
        m_editorScene = nullptr;
        for (auto panel : m_panels)
            panel.second = nullptr;
        m_panels.clear();
    }

    void Canvas::setTargetObject(const std::shared_ptr<SceneObject>& obj)
    {
        m_inspector->setTargetObject(obj);
        m_editorScene->setTargetObject(obj);
    }

    void Canvas::update(float dt)
    {
        for (auto panel : m_panels)
            panel.second->update(dt);
    }

    void Canvas::draw()
    {
        ImGui::NewFrame();

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

        ImGui::Begin("##Workspace", nullptr, flags);
        m_menuBar->draw();
        m_toolBar->draw();

        if (!m_panels.empty())
        {
            if (isDockable())
            {
                ImVec2 displaySize = ImGui::GetIO().DisplaySize;
                ImGui::SetWindowPos({ 0.f, 0.f });
                ImGui::SetWindowSize({ (float)displaySize.x, (float)displaySize.y });

                if (ImGui::DockBuilderGetNode(ImGui::GetID("EditorDockspace")) == NULL)
                {
                    ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");
                    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
                    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
                    ImGui::DockBuilderSetNodeSize(dockspace_id, displaySize);

                    ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.                    
                    ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
                    ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
                    ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);
                    ImGuiID dock_id_right_bottom = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.22f, NULL, &dock_id_right);
                    
                    ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
                    ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                    ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
                    ImGui::DockBuilderDockWindow("AssetBrowser", dock_id_bottom);
                    ImGui::DockBuilderDockWindow("Scene", dock_main_id);
                    ImGui::DockBuilderDockWindow("Game", dock_main_id);
                    ImGui::DockBuilderFinish(dockspace_id);
                }

                ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");
                ImGui::DockSpace(dockspace_id, displaySize, ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);
            }

            for (auto panel : m_panels)
            {
                panel.second->draw();
            }
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