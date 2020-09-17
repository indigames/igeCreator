#pragma once

#include <memory>
#include <unordered_map>

#include "core/IDrawable.h"

#include <scene/SceneObject.h>
using namespace ige::scene;

namespace ige::creator
{
    class Panel;
    class MenuBar;
    class ToolBar;
    class Inspector;
    class EditorScene;
    class GameScene;
    class Hierarchy;
    class Console;

    class Canvas: public IDrawable
    {
    public:
        Canvas();
        virtual ~Canvas();

        virtual void update(float dt);
        virtual void draw() override;

        void setTargetObject(const std::shared_ptr<SceneObject>& obj);

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
        std::shared_ptr<Inspector>& getInspector() { return m_inspector; }
        std::shared_ptr<EditorScene>& getEditorScene() { return m_editorScene; }
        std::shared_ptr<GameScene>& getGameScene() { return m_gameScene; }
        std::shared_ptr<Hierarchy>& getHierarchy() { return m_hierarchy; }
        std::shared_ptr<Console>& getConsole() { return m_console; }

    protected:
        bool m_bDockable = false;
        std::shared_ptr<MenuBar> m_menuBar = nullptr;
        std::shared_ptr<ToolBar> m_toolBar = nullptr;
        std::shared_ptr<Inspector> m_inspector = nullptr;
        std::shared_ptr<EditorScene> m_editorScene = nullptr;
        std::shared_ptr<GameScene> m_gameScene = nullptr;
        std::shared_ptr<Hierarchy> m_hierarchy = nullptr;
        std::shared_ptr<Console> m_console = nullptr;
        std::unordered_map<std::string, std::shared_ptr<Panel>> m_panels;
    };
}
