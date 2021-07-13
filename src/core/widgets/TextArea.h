#pragma once

#include "core/Widget.h"
#include <array>

namespace ige::creator
{
    class TextArea : public DataWidget<std::string>
    {
    protected:
        static const int MAX_TEXT_LENGHT = 512;
    public:
        TextArea(const std::string& label, const std::string& content = {}, const ImVec2& size = { 32.f, 32.f }, bool readOnly = false);
        virtual ~TextArea();

        ImVec2 getSize() const { return m_size; }
        void setSize(const ImVec2& size) { m_size = size; }

    protected:
        virtual void _drawImpl() override;

    protected:

        std::string m_label;
        std::array<char, MAX_TEXT_LENGHT> m_content;
        ImVec2 m_size = ImVec2(32.f, 32.f);

        bool m_bIsReadOnly;
    };
}
