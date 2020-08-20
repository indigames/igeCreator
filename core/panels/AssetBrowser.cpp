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

#include "utils/GraphicsHelper.h"
#include <imgui_internal.h>
#include "SDL.h"

namespace ige::creator
{
    AssetBrowser::AssetBrowser(const std::string& name, const Panel::Settings& settings, const std::string& engineAssetPath, const std::string& projectAssetPath)
        : Panel(name, settings), m_engineAssetFolder(engineAssetPath), m_projectAssetFolder(projectAssetPath)
    {
            
    }
    
    AssetBrowser::~AssetBrowser()
    {
        removeAllWidgets();
    }

    void AssetBrowser::initialize()
    {
        createWidget<FileSystemWidget>();
    }

    void AssetBrowser::_drawImpl()
    {
        Panel::_drawImpl();
    }
}
