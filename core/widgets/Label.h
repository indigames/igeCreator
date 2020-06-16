#pragma once

#include <imgui.h>

#include "core/Widget.h"

namespace ige::creator
{
    class Label: public Widget
    {
    public:
        Label(const std::string& label, const ImVec4& color = {1.0f, 1.0f, 1.0f, 1.0f});
        virtual ~Label();

        void setLabel(const std::string& label) { m_label = label; }
        void serColor(const ImVec4& color) { m_color = color; }

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        ImVec4 m_color;
    };
}
