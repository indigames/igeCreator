#include <imgui.h>
#include <imgui_internal.h>

#include "core/widgets/Button.h"

namespace ige::creator
{
    Button::Button(const std::string& label, const ImVec2& size, bool enable, bool eol)
        : Widget(enable, eol), m_label(label), m_size(size)
    {
    }

    Button::Button(const ImVec4& color, const ImVec2& size, bool enable, bool eol)
        : Widget(enable, eol), m_color(color), m_size(size)
    {
    }

    Button::Button(uint32_t textureId, const ImVec2& size, bool enable, bool eol)
        : Widget(enable, eol), m_textureId(textureId), m_size(size)
    {
    }

    Button::~Button()
    {
    }

    void Button::_drawImpl()
    {
        if (m_textureId != 0xffffff && ImGui::ImageButton((ImTextureID)m_textureId, m_size))
        {
            getOnClickEvent().invoke(this);
        }
        else if (m_color.x >= 0.f && ImGui::ColorButton((m_label + m_id).c_str(), m_color, 0, m_size))
        {
            getOnClickEvent().invoke(this);
        }
        else if (m_label != "" && ImGui::ButtonEx((m_label + m_id).c_str(), m_size, !m_bEnabled ? ImGuiButtonFlags_Disabled : 0))
        {
            getOnClickEvent().invoke(this);
        }
    }
}
