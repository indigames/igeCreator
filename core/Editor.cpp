#include "core/Editor.h"
#include "core/Canvas.h"

namespace ige::creator
{
    Editor::Editor()
    {
        m_canvas = std::make_shared<Canvas>();
        initialize();
    }
    
    Editor::~Editor()
    {
        m_canvas = nullptr;
    }
    
    void Editor::initialize()
    {
        //m_canvas->createPanel<MenuBar>("Menu");
        m_canvas->setDockable(true);
    }

    void Editor::handleEvent(const void* event)
    {
    
    }

    void Editor::update(float dt) {}
            
    void Editor::render() {}   
}
