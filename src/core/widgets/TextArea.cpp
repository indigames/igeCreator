#include <imgui.h>
#include "core/widgets/TextArea.h"

namespace ige::creator
{
    TextArea::TextArea(const std::string& label, const std::string& content, const ImVec2& size, bool readOnly)
        : DataWidget<std::string>(content), m_label(label), m_bIsReadOnly(readOnly), m_size(size)
    {
        m_content.fill(0);
        std::memcpy(m_content.data(), content.c_str(), content.length() < MAX_TEXT_LENGHT ? content.length() : MAX_TEXT_LENGHT);
    }

    TextArea::~TextArea()
    {

    }

    void TextArea::_drawImpl()
    {
        int flag = ImGuiInputTextFlags_AutoSelectAll | !ImGuiInputTextFlags_EnterReturnsTrue;
        if (m_bIsReadOnly)
        {
            flag |= ImGuiInputTextFlags_ReadOnly;
        }

        bool enterPressed = ImGui::InputTextMultiline((m_label + getIdAString()).c_str(), m_content.data(), MAX_TEXT_LENGHT, m_size, flag);

        if (enterPressed)
        {
            notifyChange(m_content.data());
        }
    }
}
