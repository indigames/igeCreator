#pragma once
#include <map>
#include "core/Widget.h"

namespace ige::creator
{
    class ComboBox: public DataWidget<int>
    {
    public:
        ComboBox(const int& selectedIdx = 0, bool enable = true, bool eol = true);
        virtual ~ComboBox();

        void addChoice(int idx, const std::string& item);
        int getSelectedIndex() { return m_selectedIdx; }

    protected:
        virtual void _drawImpl() override;

        std::map<int, std::string> m_choices;
        int m_selectedIdx;
    };
}
