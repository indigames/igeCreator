#pragma once

#include <string>

#include <portable-file-dialogs.h>

namespace ige::creator
{
    //! class OpenFileDialog
    // Usage: auto selectedFiles = OpenFileDialog("Open", ".").result();
    class OpenFileDialog: public pfd::open_file
    {
    public:
        typedef pfd::opt Option;

        OpenFileDialog(std::string const &title,
            std::string const &defaultPath = "",
            std::vector<std::string> filters = { "All Files", "*" },
            Option opt = Option::none
        );
    };

    //! class OpenFolderDialog
    // Usage: auto path = OpenFolderDialog("Open", ".").result();
    class OpenFolderDialog : public pfd::select_folder
    {
    public:
        typedef pfd::opt Option;

        OpenFolderDialog(std::string const& title,
            std::string const& defaultPath = "",
            Option opt = Option::none
        );
    };
}
