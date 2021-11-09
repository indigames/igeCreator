#pragma once
#include "core/Widget.h"
#include <array>

namespace ige::creator
{
    static const int MAX_TEXT_LENGHT = 512;
    class TextField: public DataWidget<std::string>
    {
    public:
        TextField(const std::string& label, const std::string& content = {}, bool readOnly = false, bool inputWithEnter = false);
        virtual ~TextField();

        //! Get/set the text
        void setText(const std::string& text);
        std::string getText();

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        std::array<char, MAX_TEXT_LENGHT> m_content = {0};
        bool m_bIsReadOnly;
        bool m_bInputWithEnter;
    };
}
