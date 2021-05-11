#include <imgui.h>

#include "core/panels/AssetBrowser.h"

#include "core/widgets/Button.h"
#include "core/widgets/TreeNode.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Icon.h"
#include "core/widgets/Label.h"
#include "core/widgets/FileSystemWidget.h"
#include "core/layout/Group.h"
#include "core/layout/Columns.h"
#include "core/plugin/DragDropPlugin.h"

#include "core/dialog/MsgBox.h"
#include "core/dialog/OpenFileDialog.h"

#include "core/Editor.h"

#include "utils/GraphicsHelper.h"
#include <imgui_internal.h>
#include "SDL.h"

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
    }

    void AssetBrowser::_drawImpl()
    {
        Panel::_drawImpl();
    }
    
    void AssetBrowser::setDirty()
    {
        std::static_pointer_cast<FileSystemWidget>(m_fileSystemWidget)->setDirty();
    }
}
