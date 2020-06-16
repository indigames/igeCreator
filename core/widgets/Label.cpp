#include <imgui.h>

#include "core/widgets/Label.h"

namespace ige::creator
{
    Label::Label(const std::string& label, const ImVec4& color)
        : m_label(label), m_color(color)
    {
    }

    Label::~Label()
    {
    }

    void Label::_drawImpl()
    {
        ImGui::TextColored(m_color, m_label.c_str());
    }
}
