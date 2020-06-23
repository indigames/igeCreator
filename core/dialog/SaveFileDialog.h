#pragma once

#include <string>

#include "3rd-party/pfd/portable-file-dialogs.h"

namespace ige::creator
{
    //! class OpenFileDialog
    // Usage: auto savedFile = SaveFileDialog("Save", ".").result();
    class SaveFileDialog: public pfd::save_file
    {
    public:
        SaveFileDialog(std::string const &title,
              std::string const &defaultPath = "",
              std::vector<std::string> filters = { "All Files", "*" });
    };
}
