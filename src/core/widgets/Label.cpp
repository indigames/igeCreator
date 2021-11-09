#include "core/widgets/Label.h"

#include <imgui.h>

namespace ige::creator
{
    Label::Label(const std::string& label, const ImVec4& color, bool enable, bool eol)
        : Widget(enable, eol), m_label(label), m_color(color)
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
