#include <imgui.h>

#include "core/widgets/TextField.h"

namespace ige::creator
{
    TextField::TextField(const std::string& label, const char* content, bool needEnter, bool readOnly)
        : DataWidget<std::string>(content), m_label(label), m_bIsReadOnly(readOnly), m_bIsNeedEnter(needEnter)
    {
        m_content.fill(0);
        std::memcpy(m_content.data(), content, strlen(content) < MAX_TEXT_LENGHT ? strlen(content) : MAX_TEXT_LENGHT);
    }

    TextField::~TextField()
    {

    }

    void TextField::_drawImpl()
    {
        int flag = ImGuiInputTextFlags_AutoSelectAll;
        if (m_bIsNeedEnter)
        {
            flag |= ImGuiInputTextFlags_EnterReturnsTrue;
        }

        if (m_bIsReadOnly)
        {
            flag |= ImGuiInputTextFlags_ReadOnly;
        }

        bool enterPressed = ImGui::InputText((m_label + m_id).c_str(), m_content.data(), MAX_TEXT_LENGHT, flag);

       if (enterPressed)
        {           
            this->notifyChange(m_content.data());
        }
    }
}
