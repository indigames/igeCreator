#pragma once

#include <imgui.h>
#include "core/Widget.h"

namespace ige::creator
{
    class Button: public Widget
    {
    public:
        //! Constructor: Button with label
        Button(const std::string& label = "Btn", const ImVec2& size = {32.f, 32.f}, bool enable = true, bool eol = true);

        //! Constructor: Color button
        Button(const ImVec4& color);

        //! Constructor: Button with texture
        Button(uint32_t textureId, const ImVec2& size);

        virtual ~Button();
              
        std::string getLabel() const { return m_label; }
        void setLabel(const std::string& label) { m_label = label; }

        ImVec2 getSize() const { return m_size; }
        void setSize(const ImVec2& size) { m_size = size; }

        ImVec4 getColor() const { return m_color; }
        void setColor(const ImVec4& color) { m_color = color; }

        uint32_t getTextureId() const { return m_textureId; }
        void setTextureId(uint32_t textureId) { m_textureId = textureId; }

    protected:
        virtual void _drawImpl();

    private:
        std::string m_label = "";
        ImVec2 m_size = ImVec2(32.f, 32.f);
        ImVec4 m_color = ImVec4(-1.f, -1.f, -1.f, 1.f);
        uint32_t m_textureId = 0xffffff;
    };
}
