#pragma once
#include <map>
#include "core/Widget.h"

namespace ige::creator
{
    class ComboBox: public DataWidget<int>
    {
    public:
        ComboBox(const std::string& label, const int& selectedIdx = 0);
        virtual ~ComboBox();

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        std::map<int, std::string> m_choices;
        int m_selectedIdx;
    };
}
