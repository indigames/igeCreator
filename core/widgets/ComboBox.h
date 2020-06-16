#pragma once
#include <map>
#include "core/Widget.h"

namespace ige::creator
{
    class ComboBox: public DataWidget<int>
    {
    public:
        ComboBox(const std::string& label, int selectedIdx = 0);
        virtual ~ComboBox();
        
        ige::scene::Event<int>& getOnValueChangedEvent() { return m_onValueChangedEvent; }

    protected:
        virtual void _drawImpl() override;
        
        ige::scene::Event<int> m_onValueChangedEvent;

        std::string m_label;
        std::map<int, std::string> m_choices;
        int m_selectedIdx;
    };
}
