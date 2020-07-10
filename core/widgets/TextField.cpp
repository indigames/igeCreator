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
        int flag = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        if (m_bIsReadOnly)
        {
            flag |= ImGuiInputTextFlags_ReadOnly;
        }

        char txt[256] = { 0 };
        sprintf(txt, m_content.c_str());
        bool enterPressed = ImGui::InputText((m_label + m_id).c_str(), txt, 256, flag);
        m_content = std::string(txt);

        if (enterPressed)
        {
            this->notifyChange(m_content);
        }
    }
}
