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
#include "core/panels/ProjectSetting.h"
#include "core/panels/Profiler.h"
#include "core/panels/subpanels/BitmapFontCreator.h"

#include <scene/SceneManager.h>
using namespace ige::scene;

namespace ige::creator
{
    Canvas::Canvas()
    {
        m_menuBar = std::make_shared<MenuBar>("Menu");
        m_toolBar = std::make_shared<ToolBar>("ToolBar");

        Panel::Settings settings;
        settings.collapsable = true;
        settings.dockable = true;

        createPanel<Hierarchy>("Hierarchy", settings);
        createPanel<AssetBrowser>("Assets", settings);
        createPanel<Console>("Console", settings);
        createPanel<ProjectSetting>("Settings", settings);
        createPanel<EditorScene>("Scene", settings);
        createPanel<GameScene>("Game", settings);
        createPanel<Profiler>("Profiler", settings);
        createPanel<Inspector>("Inspector", settings);
        createPanel<AssetViewer>("Asset", settings);
        auto bitmapPanel = createPanel<BitmapFontCreator>("BitmapFontCreator", settings);
        bitmapPanel->close();
    }

    Canvas::~Canvas()
    {
        m_menuBar = nullptr;
        m_toolBar = nullptr;

        for (auto& [key, value] : m_panels)
            value = nullptr;
        m_panels.clear();
    }

    void Canvas::update(float dt)
    {
        int size = m_panels.size();
        if (size > 0) {
            for (const auto& [key, value] : m_panels) {
                if (value != nullptr)
                    value->update(dt);
            }
            /*for (auto x : m_panels) {
                if (x.second != nullptr) {
                    x.second->update(dt);

                }
            }*/
        }
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
                    ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.17f, NULL, &dock_main_id);
                    ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, NULL, &dock_main_id);
                    ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, NULL, &dock_main_id);
                    ImGuiID dock_id_left_bottom = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.21f, NULL, &dock_id_left);
                    ImGuiID dock_id_right_bottom = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.21f, NULL, &dock_id_right);

                    ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
                    ImGui::DockBuilderDockWindow("Settings", dock_id_right);
                    ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                    ImGui::DockBuilderDockWindow("Asset", dock_id_right_bottom);
                    ImGui::DockBuilderDockWindow("Console", dock_id_bottom);
                    ImGui::DockBuilderDockWindow("Assets", dock_id_bottom);
                    ImGui::DockBuilderDockWindow("Scene", dock_main_id);
                    ImGui::DockBuilderDockWindow("Game", dock_main_id);
                    ImGui::DockBuilderDockWindow("Profiler", dock_id_left_bottom);
                    ImGui::DockBuilderFinish(dockspace_id);
                }

                ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");
                ImGui::DockSpace(dockspace_id, {0, 0}, ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton);
            }

            for (const auto& [key, value] : m_panels)
                value->draw();
        }

        static bool firstFrame = true;
        if (firstFrame)
        {
            getInspector()->setFocus();
            getAssetBrowser()->setFocus();
            getEditorScene()->setFocus();
            firstFrame = false;
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