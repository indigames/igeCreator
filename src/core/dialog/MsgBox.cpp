#pragma once

#include "core/dialog/MsgBox.h"

namespace ige::creator
{
    MsgBox::MsgBox(std::string const &title, std::string const &text, EBtnLayout btnLayout, EMsgType type)
        : pfd::message(title, text, btnLayout, type)
    {}
}
