#pragma once

#include "core/Widget.h"
#include "event/Event.h"

namespace ige::creator
{    
    class MenuItem: public Widget
    {
    public:
        MenuItem(const std::string& name, const std::string& shortcut = "", bool enable = true);
        virtual ~MenuItem();

    protected:
        virtual void _drawImpl() override;
        
        std::string m_name;
        std::string m_shortcut;
    };
}