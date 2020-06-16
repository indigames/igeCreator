#pragma once

#include "core/Widget.h"

namespace ige::creator
{
    class RadioButton: public DataWidget<bool>
    {
    public:
        RadioButton(const std::string& label, bool selected = false);
        virtual ~RadioButton();

        void setSelected(bool selected);
        bool isSelected() const { return m_bIsSelected; }

        ige::scene::Event<bool>& getOnValueChangedEvent() { return m_onValueChangedEvent; }

    protected:
        virtual void _drawImpl() override;

        ige::scene::Event<bool> m_onValueChangedEvent;

        std::string m_label;
        bool m_bIsSelected;
    };
}
