#include "core/widgets/RadioButton.h"

#include <imgui.h>

namespace ige::creator
{
    RadioButton::RadioButton(const std::string& label, const bool& selected)
        : DataWidget<bool>(selected), m_label(label), m_bIsSelected(selected)
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
            this->notifyChange(m_bIsSelected);
        }
    }
    
    void RadioButton::_drawImpl()
    {
        if (ImGui::RadioButton((m_label + getIdAsString()).c_str(), m_bIsSelected))
        {            
            setSelected(true);
        }
        else
        {
            setSelected(false);
        }
    }

}
