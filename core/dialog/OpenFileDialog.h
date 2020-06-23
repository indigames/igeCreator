#pragma once

#include <string>

#include "3rd-party/pfd/portable-file-dialogs.h"

namespace ige::creator
{
    //! class OpenFileDialog
    // Usage: auto selectedFiles = OpenFileDialog("Open", ".").result();
    class OpenFileDialog: public pfd::open_file
    {
    public:
        OpenFileDialog(std::string const &title,
              std::string const &defaultPath = "",
              std::vector<std::string> filters = { "All Files", "*" });
    };
}
