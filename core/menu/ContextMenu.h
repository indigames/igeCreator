#pragma once

#include "core/menu/Menu.h"

namespace ige::creator
{
    class ContextMenu: public Menu, public IPlugin
    {
    public:
        ContextMenu(const std::string& name, bool enable = true);
        virtual ~ContextMenu();

        void execute();
        void close();

    protected:
        virtual void _drawImpl() override;
    };
}