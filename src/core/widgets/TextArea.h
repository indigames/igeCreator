#pragma once

#include "core/Widget.h"
#include <array>

namespace ige::creator
{
    class TextArea : public DataWidget<std::string>
    {
    protected:
        static const int MAX_TEXT_LENGHT = 16 * 1024;
    public:
        TextArea(const std::string& label, const std::string& content = {}, const ImVec2& size = { -1.f, -1.f }, bool readOnly = false, bool autoScroll = false);
        virtual ~TextArea();

        //! Get/set area size
        ImVec2 getSize() const { return m_size; }
        void setSize(const ImVec2& size) { m_size = size; }

        //! Get/set text
        void setText(const std::string& text);
        std::string getText();

        //! Get/set autoScroll
        void setAutoScroll(bool autoScroll = true) { m_bIsAutoScroll = autoScroll; }
        bool isAutoScroll() { return m_bIsAutoScroll; }

    protected:
        virtual void _drawImpl() override;

    protected:

        std::string m_label;
        std::array<char, MAX_TEXT_LENGHT> m_content;
        ImVec2 m_size = ImVec2(32.f, 32.f);

        bool m_bIsReadOnly = false;
        bool m_bIsAutoScroll = false;
    };
}
