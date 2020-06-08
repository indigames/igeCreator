#pragma once

#include <memory>
#include <unordered_map>

#include "core/IDrawable.h"

namespace ige::creator
{
    class Panel;
    class MenuBar;
    class ToolBar;

    class Canvas: public IDrawable
    {
    public:
        Canvas();
        virtual ~Canvas();

        virtual void draw() override;

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
            return m_panels[id];
        }

        void removePanel(std::shared_ptr<Panel> panel);
        void removePanelById(const std::string& id);

        bool isDockable() const { return m_bDockable; }
        void setDockable(bool dockable) { m_bDockable = dockable; }

    protected:
        bool m_bDockable = false;
        std::shared_ptr<MenuBar> m_menuBar = nullptr;
        std::shared_ptr<ToolBar> m_toolBar = nullptr;
        std::unordered_map<std::string, std::shared_ptr<Panel>> m_panels;
    };    
}
