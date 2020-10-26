#pragma once

#include "core/Widget.h"

namespace ige::creator
{
    class CheckBox: public DataWidget<bool>
    {
    public:
        CheckBox(const std::string& label, const bool& selected = false);
        virtual ~CheckBox();

        void setSelected(bool selected = true) { m_bIsSelected = selected; }
        bool isSelected() const { return m_bIsSelected; }

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        bool m_bIsSelected;
    };
}
