#include <imgui.h>
#include <imgui_internal.h>

#include "core/button/Button.h"

namespace ige::creator
{
    Button::Button(const std::string& label, const ImVec2& size, const ImVec4& color, bool enable)
        : Widget(enable), m_label(label), m_size(size), m_color(color)
    {
    }
    
    Button::~Button()
    {
    }

    void Button::_drawImpl()
    {
        if (m_color.x >= 0.f && ImGui::ColorButton((m_label + m_id).c_str(), m_color, 0, m_size))
        {
            m_onClickEvent.invoke();
        }
        else if(ImGui::ButtonEx((m_label + m_id).c_str(), m_size, !m_bEnabled ? ImGuiButtonFlags_Disabled : 0))
        {
             m_onClickEvent.invoke();
        }
    }
}
