#include <imgui.h>

#include "core/toolbar/ToolBar.h"

namespace ige::creator
{
    ToolBar::ToolBar(const std::string& name, bool enable)
        : Widget(enable), m_name(name)
    {
    }
    
    ToolBar::~ToolBar()
    {

    }

    void ToolBar::initialize()
    {
        
    }

    void ToolBar::_drawImpl()
    {
        static bool initialized = false;
        if (!initialized)
        {
            initialize();
            initialized = true;
        }

        // if (m_widgets.empty()) return;
       
    }    
}
