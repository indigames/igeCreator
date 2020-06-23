#pragma once

#include <imgui.h>
#include "core/Widget.h"

namespace ige::creator
{
    class Icon: public Widget
    {
    public:
        Icon(const std::string& label, uint32_t textureId, const ImVec2& size = {32.f, 32.f}, bool enable = true);
        virtual ~Icon();

        const std::string& getLabel() { return m_label; }

    protected:
        virtual void _drawImpl();

    protected:
        ImVec2 m_size;
        uint32_t m_textureId;
        std::string m_label;
    };
}
