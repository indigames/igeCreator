#pragma once

#include "core/Widget.h"

namespace ige::creator
{
    class CheckBox: public DataWidget<bool>
    {
    public:
        CheckBox(const std::string& label, bool selected = false);
        virtual ~CheckBox();

        bool isSelected() const { return m_bIsSelected; }
        ige::scene::Event<bool>& getOnValueChangedEvent() { return m_onValueChangedEvent; }

    protected:
        virtual void _drawImpl() override;

        ige::scene::Event<bool> m_onValueChangedEvent;

        std::string m_label;
        bool m_bIsSelected;
    };
}
