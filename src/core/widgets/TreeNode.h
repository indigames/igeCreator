#pragma once
#include "core/Container.h"
#include "core/Widget.h"

#include <imgui.h>

namespace ige::creator
{
    class TreeNode: public Widget, public Container
    {
    public:
        TreeNode(const std::string& name, bool isSelected = false, bool isLeaf = false, bool opened = false);
        virtual ~TreeNode();
        
        ige::scene::Event<>& getOnOpenedEvent() { return m_onOpenedEvent; }
        ige::scene::Event<>& getOnClosedEvent() { return m_onClosedEvent; }

        bool isLeaf() { return m_bIsLeaf; }
        void setIsLeaf(bool isLeaf) { m_bIsLeaf = isLeaf; }

        bool isSelected() { return m_bIsSelected; }
        void setIsSelected(bool isSelected) { m_bIsSelected = isSelected; }

        const std::string& getName() { return m_name; }
        void setName(const std::string& name) { m_name = name; }

        const ImVec4& getColor() { return m_color; }
        void setColor(const ImVec4& color) { m_color = color; }

        virtual void draw() override;

        bool isOpened() { return m_opened; }
        void open();
        void close();

        bool isHighlighted() const { return m_bIsHighlighted; }
        void setHighlighted(bool highlight = true, long duration = 1000) { m_bIsHighlighted = highlight; }

    protected:
        virtual void _drawImpl() override;

        ige::scene::Event<> m_onOpenedEvent;
        ige::scene::Event<> m_onClosedEvent;

        std::string m_name;
        bool m_opened = false;
        bool m_bIsDefaultOpened = false;
        bool m_bIsSelected = false;
        bool m_bIsLeaf = false;
        bool m_shouldOpen = false;
        bool m_shouldClose = false;
        bool m_bIsHighlighted = false;
        ImVec4 m_color;
    };
}
