#include "core/widgets/Color.h"

#include <imgui.h>

namespace ige::creator
{
    Color::Color(const std::string& label, const Vec4& value)
        : DataWidget(value), m_label(label)
    {
    }

    Color::~Color()
    {
    }

    void Color::_drawImpl()
    {
        ImGui::PushID((m_label + getIdAsString()).c_str());

        if (ImGui::ColorEdit4(m_label.c_str(), m_data.P()))
        {
            getOnDataChangedEvent().invoke(m_data);
        }

        ImGui::PopID();
    }
}
