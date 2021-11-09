#pragma once
#include "core/Widget.h"

namespace ige::creator
{
    class RadioButton: public DataWidget<bool>
    {
    public:
        RadioButton(const std::string& label, const bool& selected = false);
        virtual ~RadioButton();

        void setSelected(bool selected);
        bool isSelected() const { return m_bIsSelected; }

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        bool m_bIsSelected;
    };
}
