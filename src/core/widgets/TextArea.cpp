#include <imgui.h>
#include "core/widgets/TextArea.h"

namespace ige::creator
{
    TextArea::TextArea(const std::string& label, const std::string& content, const ImVec2& size, bool readOnly, bool autoScroll)
        : DataWidget<std::string>(content), m_label(label), m_size(size), m_bIsReadOnly(readOnly), m_bIsAutoScroll(autoScroll)
    {
        m_content.fill(0);
        std::memcpy(m_content.data(), content.c_str(), content.length() < MAX_TEXT_LENGHT ? content.length() : MAX_TEXT_LENGHT);
    }

    TextArea::~TextArea()
    {

    }

    void TextArea::setText(const std::string& text)
    {
        m_content.fill(0);
        std::memcpy(m_content.data(), text.c_str(), text.length() < MAX_TEXT_LENGHT ? text.length() : MAX_TEXT_LENGHT);
    }

    std::string TextArea::getText()
    {
        return std::string(m_content.data());
    }

    void TextArea::_drawImpl()
    {
        int flag = ImGuiInputTextFlags_AutoSelectAll;
        if (m_bIsReadOnly) flag |= ImGuiInputTextFlags_ReadOnly;
        else flag |= ImGuiInputTextFlags_EnterReturnsTrue;
        if (m_bIsAutoScroll) flag |= ImGuiInputTextFlags_AutoScrolling;

        ImGui::PushID(getIdAsString().c_str());
        ImGui::PushItemWidth(ImGui::GetWindowSize().x);

        auto size = m_size;
        if (size.x <= 0) size.x = ImGui::GetWindowSize().x;
        if (size.y <= 0) size.y = ImGui::GetWindowSize().y;
        
        bool enterPressed = ImGui::InputTextMultiline((m_label + getIdAsString()).c_str(), m_content.data(), MAX_TEXT_LENGHT, size, flag);
        if (enterPressed) {
            notifyChange(m_content.data());
        }
        ImGui::PopItemWidth();
        ImGui::PopID();
    }
}
