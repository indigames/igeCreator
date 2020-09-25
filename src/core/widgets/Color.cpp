#include <imgui.h>

#include "core/widgets/Color.h"

namespace ige::creator
{
    Color::Color(const std::string& label, float* value)
        : DataWidget(value), m_label(label), m_value(value)
    {
    }

    Color::~Color()
    {
        m_value = nullptr;
    }

    void Color::_drawImpl()
    {
        ImGui::PushID((m_label + m_id).c_str());

        if (m_value && ImGui::ColorEdit4(m_label.c_str(), m_value))
        {
            getOnDataChangedEvent().invoke(m_value);
        }

        ImGui::PopID();
    }
}
