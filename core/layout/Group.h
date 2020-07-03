#pragma once

#include <string>
#include "core/Container.h"
#include "core/Widget.h"

namespace ige::creator
{
    class Group: public Widget, public Container
    {
    public:
        enum class E_Align {
            LEFT = 0,
            CENTER = 1,
            RIGHT = 2
        };

        Group(const std::string& name, bool collapsable = true, bool closable = false, E_Align align = E_Align::LEFT, bool open = true, bool enable = true);
        virtual ~Group();

        ige::scene::Event<>& getOnClosedEvent() { return m_onClosedEvent; }
        ige::scene::Event<>& getOnOpenedEvent() { return m_onOpenedEvent; }

    protected:
        virtual void _drawImpl() override;

        ige::scene::Event<> m_onClosedEvent;
        ige::scene::Event<> m_onOpenedEvent;

        std::string m_name;
        bool m_bIsOpened = true;
        bool m_bIsCollapsable = false;
        bool m_bIsClosable = false;
        E_Align m_align = E_Align::LEFT;
    };
}