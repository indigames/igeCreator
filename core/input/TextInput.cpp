#include <imgui.h>

#include "core/input/TextInput.h"
#include "core/Widget.h"

namespace ige::creator
{
    TextInput::TextInput(const std::string& label, std::string& content)
        : DataWidget<std::string>(content), m_label(label)
    {

    }

    TextInput::~TextInput()
    {

    }

    void TextInput::_drawImpl()
    {
        std::string previousContent = m_content;
        m_content.resize(512, '\0');

        bool enterPressed = ImGui::InputText((m_label + m_id).c_str(), &m_content[0], 512, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

        if (m_content != previousContent)
        {           
            this->notifyChange(m_content);
        }
    }
}
