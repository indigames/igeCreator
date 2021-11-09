#pragma once
#include "core/Widget.h"
#include <map>

namespace ige::creator
{
    class ComboBox: public DataWidget<int>
    {
    public:
        ComboBox(const std::string& label, const int& selectedIdx = 0, bool enable = true, bool eol = true);
        virtual ~ComboBox();

        void addChoice(int idx, const std::string& item);
        int getSelectedIndex() { return m_selectedIdx; }

    protected:
        virtual void _drawImpl() override;

        std::map<int, std::string> m_choices;
        int m_selectedIdx;
        std::string m_label;
    };
}
