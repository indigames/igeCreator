#include <imgui.h>

#include "core/widgets/RadioButton.h"

namespace ige::creator
{
    RadioButton::RadioButton(const std::string& label, bool selected)
        : DataWidget<bool>(selected), m_label(label)
    {
    }

    RadioButton::~RadioButton()
    {
    }

    void RadioButton::setSelected(bool selected)
    {
        if(m_bIsSelected != selected)
        {
            m_bIsSelected = selected;
            getOnValueChangedEvent().invoke(m_bIsSelected);
            this->notifyChange(m_bIsSelected);
        }
    }
    
    void RadioButton::_drawImpl()
    {
        if (ImGui::RadioButton((m_label + m_id).c_str(), m_bIsSelected))
        {            
            setSelected(true);
        }
        else
        {
            setSelected(false);
        }
    }

}
