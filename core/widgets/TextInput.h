#pragma once

#include "core/Widget.h"

namespace ige::creator
{
    class TextInput: public DataWidget<std::string>
    {
    public:
        TextInput(const std::string& label, std::string& content);
        virtual ~TextInput();


    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        std::string m_content;
    };
}
