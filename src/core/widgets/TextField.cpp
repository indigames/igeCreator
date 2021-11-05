#include <imgui.h>

#include "core/widgets/TextField.h"

namespace ige::creator
{
    TextField::TextField(const std::string& label, const std::string& content, bool readOnly, bool inputWithEnter)
        : DataWidget<std::string>(content), m_label(label), m_bIsReadOnly(readOnly), m_bInputWithEnter(inputWithEnter)
    {
        m_content.fill(0);
        std::memcpy(m_content.data(), content.c_str(), content.length() < MAX_TEXT_LENGHT ? content.length() : MAX_TEXT_LENGHT);
    }

    TextField::~TextField()
    {

    }

    void TextField::_drawImpl()
    {
        int flag = ImGuiInputTextFlags_AutoSelectAll;

        if (m_bIsReadOnly) {
            flag |= ImGuiInputTextFlags_ReadOnly;
        }

        if (m_bInputWithEnter) {
            flag |= ImGuiInputTextFlags_EnterReturnsTrue;
        }

        ImGui::PushID(getIdAsString().c_str());
        ImGui::PushItemWidth(ImGui::GetWindowSize().x);
        if (ImGui::InputText((m_label + getIdAsString()).c_str(), m_content.data(), MAX_TEXT_LENGHT, flag)) {
            notifyChange(m_content.data());
        }
        ImGui::PopItemWidth();
        ImGui::PopID();
    }
        
    void TextField::setText(const std::string& text)
    {
        m_content.fill(0);
        std::memcpy(m_content.data(), text.c_str(), text.length() < MAX_TEXT_LENGHT ? text.length() : MAX_TEXT_LENGHT);
    }

    std::string TextField::getText()
    {
        return std::string(m_content.data());
    }
}
