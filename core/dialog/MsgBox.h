#pragma once

#include <string>

#include "3rd-party/pfd/portable-file-dialogs.h"

namespace ige::creator
{
    //! class MsgBox
    // Usage: EButton btn = MsgBox("Message", "Test message").result();
    class MsgBox: public pfd::message
    {
    public:
        typedef pfd::choice EBtnLayout;
        typedef pfd::icon EMsgType;
        typedef pfd::button EButton;

        MsgBox(std::string const &title,
            std::string const &text,
            EBtnLayout btnLayout = EBtnLayout::ok_cancel,
            EMsgType type = EMsgType::info);
    };
}
