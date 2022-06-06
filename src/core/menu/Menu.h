#pragma once

#include <string>
#include "core/Container.h"
#include "core/Widget.h"

namespace ige::creator
{
    class Menu: public Widget, public Container
    {
    public:
        Menu(const std::string& name, bool enable = true);
        virtual ~Menu();

        const std::string& getName() const { return m_name; }

    protected:
        virtual void _drawImpl() override;

        std::string m_name;
        bool m_bIsOpened = false;
    };
}