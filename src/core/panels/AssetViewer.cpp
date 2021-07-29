#include <imgui.h>

#include "core/panels/AssetViewer.h"
#include "core/panels/AssetBrowser.h"

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/FileHandle.h"
#include "core/scene/assets/FigureMeta.h"

namespace ige::creator
{
    AssetViewer::AssetViewer(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings) {
        close();
        m_assetGroup = createWidget<Group>("Asset", false, false);
    }

    AssetViewer::~AssetViewer() {
        m_assetGroup->removeAllWidgets();
        m_assetGroup = nullptr;
    }

    void AssetViewer::initialize() {
        clear();
    }

    void AssetViewer::_drawImpl() {
        auto path = Editor::getCanvas()->getAssetBrowser()->getSelectedPath();
        if (m_path.compare(path) != 0) {
            m_path = path;
            clear();
            auto fsPath = fs::path(m_path);
            auto metaPath = fsPath.parent_path().append(fsPath.filename().string() + ".meta");
            if (fs::exists(metaPath)) {
                if(IsFormat(E_FileExts::Figure, fsPath.extension()) == 0)
                {
                    m_assetMeta = std::make_shared<FigureMeta>(m_path);
                } else {
                    m_assetMeta = std::make_shared<AssetMeta>(m_path);
                }
                m_assetMeta->draw(m_assetGroup);
                open();
            } else {
                close();
            }
        }
        Panel::_drawImpl();
    }

    void AssetViewer::clear() {
        m_assetGroup->removeAllWidgets();
        m_assetMeta = nullptr;
    }
}
