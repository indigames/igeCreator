#include <imgui.h>

#include "core/panels/EditorScene.h"

namespace ige::creator
{
    EditorScene::EditorScene(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
    }
    
    EditorScene::~EditorScene()
    {

    }

    void EditorScene::initialize()
    {
        clear();
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void EditorScene::clear()
    {

    }
}
