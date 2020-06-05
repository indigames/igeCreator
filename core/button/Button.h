#pragma once

#include <imgui.h>
#include "core/Widget.h"
#include "event/Event.h"

namespace ige::creator
{
    class Button: public Widget
    {
    public:
        Button(const std::string& label = "", const ImVec2& size = {0.f, 0.f}, const ImVec4& color = {-1.f, 0.f, 0.f, 0.f}, bool enable = true);
        virtual ~Button();
              
        std::string getLabel() const { return m_label; }
        void setLabel(const std::string& label) { m_label = label; }

        ImVec2 getSize() const { return m_size; }
        void setSize(const ImVec2& size) { m_size = size; }

        ImVec4 getColor() const { return m_color; }
        void setColor(const ImVec4& color) { m_color = color; }

    protected:
        virtual void _drawImpl();

    private:
        std::string m_label;
        ImVec2 m_size;
        ImVec4 m_color;
    };
}