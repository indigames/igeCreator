#pragma once

#include <event/Event.h>
using namespace ige::scene;

#include "core/Container.h"
#include "core/Widget.h"

namespace ige::creator
{
    class TreeNode: public Widget, public Container
    {
    public:
        TreeNode(const std::string& name, bool isSelected = false, bool isLeaf = false);
        virtual ~TreeNode();
        
        Event<>& getOnOpenedEvent() { return m_onOpenedEvent; }
		Event<>& getOnClosedEvent() { return m_onClosedEvent; }
        
        void setIsLeaf(bool isLeaf) { m_bIsLeaf = isLeaf; }
        void setIsSelected(bool isSelected) { m_bIsSelected = isSelected; }

    protected:
        virtual void _drawImpl() override;

        Event<> m_onOpenedEvent;
		Event<> m_onClosedEvent;

        std::string m_name;
        bool m_opened = false;
        bool m_bIsSelected = false;
        bool m_bIsLeaf = false;
    };
}
