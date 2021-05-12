#pragma once

#include "core/Widget.h"
#include "core/Container.h"

namespace ige::creator
{
    class MenuItem;

    class MenuBar: public Widget, public Container
    {
    public:
        MenuBar(const std::string& name = "", bool enable = true);
        virtual ~MenuBar();
        virtual void initialize();

    protected:
        virtual void _drawImpl();
    
    private:
        void createFileMenu();

    private:
        std::string m_name;
    };
}