#pragma once

#include <memory>
#include <unordered_map>

#include "core/IDrawable.h"

namespace ige::creator
{
    class Panel;
    class Canvas: public IDrawable
    {
    public:
        Canvas();
        virtual ~Canvas();

        virtual void draw() override;

        template<typename T, typename... Args>
        void createPanel(const std::string& id, Args&&... args);

        template<typename T>
        std::shared_ptr<T> getPanelAs(const std::string& id);

        void removePanel(std::shared_ptr<Panel> panel);
        void removePanelById(const std::string& id);

        bool isDockable() const { return m_bDockable; }
        void setDockable(bool dockable) { m_bDockable = dockable; }

    protected:
        bool m_bDockable = false;
        std::unordered_map<std::string, std::shared_ptr<Panel>> m_panels;
    };    
}
