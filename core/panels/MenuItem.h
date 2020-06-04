#pragma once

#include "core/Widget.h"
#include "event/Event.h"

namespace ige::creator
{    
    class MenuItem: public Widget
    {
    public:
        MenuItem(const std::string& name, const std::string& shortcut = "");
        virtual ~MenuItem();

        Event<>& getClickedEvent() { return m_clickEvent; }
        
    protected:
        virtual void _drawImpl() override;

        Event<> m_clickEvent;
        std::string m_name;
        std::string m_shortcut;
    };
}