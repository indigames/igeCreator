#include <imgui.h>

#include "core/widgets/TextField.h"

namespace ige::creator
{
    TextField::TextField(const std::string& label, const std::string& content, bool readOnly)
        : DataWidget<std::string>(content), m_label(label), m_content(content), m_bIsReadOnly(readOnly)
    {
    }

    TextField::~TextField()
    {

    }

    void TextField::_drawImpl()
    {
        std::string previousContent = m_content;
        m_content.resize(512, '\0');


        int flag = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        if (m_bIsReadOnly)
        {
            flag |= ImGuiInputTextFlags_ReadOnly;
        }

        bool enterPressed = ImGui::InputText((m_label + m_id).c_str(), &m_content[0], 512, flag);

        if (m_content != previousContent)
        {           
            this->notifyChange(m_content);
        }
    }
}
