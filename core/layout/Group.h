#pragma once

#include <string>
#include "core/Container.h"
#include "core/Widget.h"

namespace ige::creator
{
    class Group: public Widget, public Container
    {
    public:
        Group(const std::string& name, bool collapsable = true, bool closable = false, bool enable = true);
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
    };
}