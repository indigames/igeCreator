#include "core/dialog/OpenFileDialog.h"

namespace ige::creator
{
    OpenFileDialog::OpenFileDialog(std::string const &title, std::string const &defaultPath, std::vector<std::string> filters)
        : pfd::open_file(title, defaultPath, filters)
    {}
}
