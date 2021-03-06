#include "core/dialog/OpenFileDialog.h"

namespace ige::creator
{
    OpenFileDialog::OpenFileDialog(std::string const &title, std::string const &defaultPath, std::vector<std::string> filters, OpenFileDialog::Option opt)
        : pfd::open_file(title, defaultPath, filters, opt)
    {}

    OpenFolderDialog::OpenFolderDialog(std::string const& title, std::string const& defaultPath, OpenFileDialog::Option opt)
        : pfd::select_folder(title, defaultPath, opt)
    {}
}
