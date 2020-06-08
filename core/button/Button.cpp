#include <imgui.h>
#include <imgui_internal.h>

#include "core/button/Button.h"

namespace ige::creator
{
    Button::Button(const std::string& label, const ImVec2& size, bool enable)
        : Widget(enable), m_label(label), m_size(size)
    {
    }
    
    Button::Button(const ImVec4& color)
        : Widget(true), m_color(color)
    {
    }

    Button::Button(uint32_t textureId, const ImVec2& size)
         : Widget(true), m_textureId(textureId), m_size(size)
    {
    }

    Button::~Button()
    {
    }

    void Button::_drawImpl()
    {
        if((m_textureId != 0xffffff && ImGui::ImageButton((ImTextureID)m_textureId, m_size))
            || (m_color.x >= 0.f && ImGui::ColorButton((m_label + m_id).c_str(), m_color, 0, m_size))
            || (ImGui::ButtonEx((m_label + m_id).c_str(), m_size, !m_bEnabled ? ImGuiButtonFlags_Disabled : 0))
        )
        {
		    m_onClickEvent.invoke();
        }
    }
}
