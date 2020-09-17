#pragma once

#include <imgui.h>
#include "core/Widget.h"

namespace ige::creator
{
    class Image: public Widget
    {
    public:
        Image(uint32_t textureId, const ImVec2& size = {32.f, 32.f}, bool enable = true, bool eol = true);
        virtual ~Image();

        uint32_t getTextureId() const { return m_textureId; }
        void setTextureId(uint32_t textureId) { m_textureId = textureId; }

        ImVec2 getSize() const { return m_size; }
        void setSize(const ImVec2& size) { m_size = size; }

    protected:
        virtual void _drawImpl();

    protected:
        ImVec2 m_size;
        uint32_t m_textureId;
    };
}
