#include <imgui.h>

#include "core/widgets/Color.h"

namespace ige::creator
{
    Color::Color(const std::string& label, float* value)
        : m_label(label), m_value(value)
    {
    }

    Color::~Color()
    {
    }

    void Color::_drawImpl()
    {
        ImGui::PushID((m_label + m_id).c_str());
        ImGui::ColorEdit4(m_label.c_str(), m_value);
        ImGui::PopID();
    }
}
