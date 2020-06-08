#include <imgui.h>

#include "core/panels/AssetViewer.h"

namespace ige::creator
{
    AssetViewer::AssetViewer(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
    }
    
    AssetViewer::~AssetViewer()
    {

    }

    void AssetViewer::initialize()
    {
        clear();
    }

    void AssetViewer::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void AssetViewer::clear()
    {

    }
}
