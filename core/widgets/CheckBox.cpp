#include <imgui.h>

#include "core/widgets/CheckBox.h"

namespace ige::creator
{
    CheckBox::CheckBox(const std::string& label, const bool& selected)
        : DataWidget<bool>(selected), m_label(label), m_bIsSelected(selected)
    {
    }

    CheckBox::~CheckBox()
    {
    }

    void CheckBox::_drawImpl()
    {
        bool prevValue = m_bIsSelected;
        ImGui::Checkbox((m_label + m_id).c_str(), &m_bIsSelected);

        if(prevValue != m_bIsSelected)
        {
            this->notifyChange(m_bIsSelected);
        }       
    }

}
