#pragma once

#include <string>

#include "core/Container.h"
#include "core/Widget.h"
#include "event/Event.h"

namespace ige::creator
{
    class Menu: public Widget, public Container
    {
    public:
        Menu(const std::string& name);
        virtual ~Menu();

    protected:
        virtual void _drawImpl() override;

        std::string m_name;
        Event<> m_clickedEvent;
        bool m_bIsOpened = false;
    };
}