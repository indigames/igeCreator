#include "core/dialog/SaveFileDialog.h"

namespace ige::creator
{
    SaveFileDialog::SaveFileDialog(std::string const &title, std::string const &defaultPath, std::vector<std::string> filters)
        : pfd::save_file(title, defaultPath, filters)
    {}
}
