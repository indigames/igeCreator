#pragma once

#include <memory>
#include <unordered_map>

#include "core/IDrawable.h"

#include <scene/SceneObject.h>
using namespace ige::scene;

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
#include "core/panels/AnimatorEditor.h"

namespace ige::creator
{
    class Canvas: public IDrawable
    {
    public:
        Canvas();
        virtual ~Canvas();

        virtual void update(float dt);
        virtual void draw() override;

        void onPanelFocus(Panel& panel);

        template<typename T, typename... Args>
        std::shared_ptr<T> createPanel(const std::string& id, Args&&... args)
        {
            if constexpr (std::is_base_of<Panel, T>::value)
            {
                auto panel = std::make_shared<T>(id, std::forward<Args>(args)...);
                m_panels.emplace(id, panel);
                return panel;
            }
            return nullptr;
        }

        template<typename T>
        std::shared_ptr<T> getPanelAs(const std::string& id)
        {
            return std::dynamic_pointer_cast<T>(m_panels[id]);
        }

        void removePanel(std::shared_ptr<Panel> panel);
        void removePanelById(const std::string& id);

        bool isDockable() const { return m_bDockable; }
        void setDockable(bool dockable) { m_bDockable = dockable; }

        std::shared_ptr<MenuBar>& getMenuBar() { return m_menuBar; }
        std::shared_ptr<ToolBar>& getToolBar() { return m_toolBar; }

        std::shared_ptr<Inspector> getInspector() { return getPanelAs<Inspector>("Inspector"); }
        std::shared_ptr<EditorScene> getEditorScene() { return getPanelAs<EditorScene>("Scene"); }
        std::shared_ptr<GameScene> getGameScene() { return getPanelAs<GameScene>("Preview"); }
        std::shared_ptr<Hierarchy> getHierarchy() { return getPanelAs<Hierarchy>("Hierarchy"); }
        std::shared_ptr<Console> getConsole() { return getPanelAs<Console>("Console"); }
        std::shared_ptr<AssetBrowser> getAssetBrowser() { return getPanelAs<AssetBrowser>("Assets"); }
        std::shared_ptr<ProjectSetting> getProjectSetting() { return getPanelAs<ProjectSetting>("Settings"); }
        std::shared_ptr<AnimatorEditor> getAnimatorEditor() { return getPanelAs<AnimatorEditor>("Animator"); }

    protected:
        bool m_bDockable = false;
        std::shared_ptr<MenuBar> m_menuBar = nullptr;
        std::shared_ptr<ToolBar> m_toolBar = nullptr;
        std::unordered_map<std::string, std::shared_ptr<Panel>> m_panels;
    };
}
