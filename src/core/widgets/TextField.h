#pragma once

#include "core/Widget.h"
#include <array>

namespace ige::creator
{
    static const int MAX_TEXT_LENGHT = 128;
    class TextField: public DataWidget<std::string>
    {
    public:
        TextField(const std::string& label, const char* content, bool needEnter = false,  bool readOnly = false);
        virtual ~TextField();

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        std::array<char, MAX_TEXT_LENGHT> m_content;
        bool m_bIsReadOnly;
        bool m_bIsNeedEnter;
    };
}
