#pragma once

#include "core/Container.h"
#include "core/Widget.h"

namespace ige::creator
{
    class TreeNode: public Widget, public Container
    {
    public:
        TreeNode(const std::string& name, bool isSelected = false, bool isLeaf = false, bool opened = false);
        virtual ~TreeNode();
        
        ige::scene::Event<>& getOnOpenedEvent() { return m_onOpenedEvent; }
        ige::scene::Event<>& getOnClosedEvent() { return m_onClosedEvent; }
        
        void setIsLeaf(bool isLeaf) { m_bIsLeaf = isLeaf; }
        void setIsSelected(bool isSelected) { m_bIsSelected = isSelected; }

        const std::string& getName() { return m_name; }
        void setName(const std::string& name) { m_name = name; }

        virtual void draw() override;

    protected:
        virtual void _drawImpl() override;

        ige::scene::Event<> m_onOpenedEvent;
        ige::scene::Event<> m_onClosedEvent;

        std::string m_name;
        bool m_opened = false;
        bool m_bIsDefaultOpened = false;
        bool m_bIsSelected = false;
        bool m_bIsLeaf = false;
    };
}
