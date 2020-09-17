#pragma once

#include "core/menu/Menu.h"

namespace ige::creator
{
    class ContextMenu: public Menu, public IPlugin
    {
    public:
        ContextMenu(const std::string& name, bool enable = true);
        virtual ~ContextMenu();

        virtual void execute();
        void close();

    protected:
        virtual void _drawImpl() override;
    };

    class WindowContextMenu : public ContextMenu
    {
    public:
        WindowContextMenu(const std::string& name, bool enable = true);
        virtual ~WindowContextMenu() {}
      
    protected:
        virtual void execute() override;
    };
}