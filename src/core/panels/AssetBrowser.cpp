#include <imgui.h>
#include "core/panels/AssetBrowser.h"

#include "core/widgets/Button.h"
#include "core/widgets/TreeNode.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Icon.h"
#include "core/widgets/Label.h"
#include "core/widgets/FileSystemWidget.h"
#include "core/FileHandle.h"
#include "core/layout/Group.h"
#include "core/layout/Columns.h"
#include "core/plugin/DragDropPlugin.h"

#include "core/dialog/MsgBox.h"
#include "core/dialog/OpenFileDialog.h"

#include "core/Editor.h"

#include "utils/GraphicsHelper.h"
#include <imgui_internal.h>
#include "SDL.h"

#include <iomanip>

namespace ige::creator
{
    AssetBrowser::AssetBrowser(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        m_bLastFocused = false;
    }
    
    AssetBrowser::~AssetBrowser()
    {
        m_fileSystemWidget = nullptr;
        removeAllWidgets();
        getOnFocusEvent().removeAllListeners();
    }

    void AssetBrowser::initialize()
    {
        m_fileSystemWidget = createWidget<FileSystemWidget>();
        for (const auto& widget : m_widgets) {
            if (widget != nullptr) {
                widget.get()->getOnClickEvent().addListener([this](Widget*) {
                    m_focusEvent.invoke();
                });
            }
        }
        m_cache.set_scan_frequency(std::chrono::milliseconds(1000));
    }

    void AssetBrowser::_drawImpl()
    {
            for(const auto& file : m_cache) {
            const auto& absPath = file.entry.path();
            const auto& name = file.stem;
            const auto& relative = file.protocol_path;
            const auto& file_ext = file.extension;
            const std::string& filename = file.filename;

            auto hidden = false;
            auto path = absPath.string();
            const auto& hiddenItems = GetFileExtensionSuported(E_FileExts::Hidden);
            for (const auto& item : hiddenItems) {
                if (path.find_first_of(item) != std::string::npos) {
                    hidden = true;
                    break;
                }
            }

            if (hidden || IsFormat(E_FileExts::Hidden, name) || IsFormat(E_FileExts::Hidden, file_ext))
                continue;

            if (fs::is_regular_file(file.entry.status()) && file_ext.compare(".meta") != 0) {
                const auto metaPath = absPath.parent_path().append(filename + ".meta");
                if (!fs::exists(metaPath)) {
                    json metaJson = json {
                        {"name", filename },
                    };
                    std::ofstream file(metaPath);
                    file << std::setw(2) << metaJson << std::endl;
                    file.close();
                }
            }
 	    }

        Panel::_drawImpl();
    }
    
    void AssetBrowser::setDirty()
    {
        m_cache.set_path(Editor::getInstance()->getProjectPath());
        std::static_pointer_cast<FileSystemWidget>(m_fileSystemWidget)->setDirty();
    }
  
    std::string AssetBrowser::getSelectedPath()
    {
        return std::static_pointer_cast<FileSystemWidget>(m_fileSystemWidget)->getSelectedPath();
    }
}
