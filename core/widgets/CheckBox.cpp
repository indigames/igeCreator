#include <imgui.h>

#include "core/widgets/CheckBox.h"

namespace ige::creator
{
    CheckBox::CheckBox(const std::string& label, bool selected)
        : DataWidget<bool>(selected), m_label(label)
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
            getOnValueChangedEvent().invoke(m_bIsSelected);
            this->notifyChange(m_bIsSelected);
        }       
    }

}
