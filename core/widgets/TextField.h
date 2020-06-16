#pragma once

#include "core/Widget.h"

namespace ige::creator
{
    class TextField: public DataWidget<std::string>
    {
    public:
        TextField(const std::string& label, std::string& content);
        virtual ~TextField();

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        std::string m_content;
    };
}
