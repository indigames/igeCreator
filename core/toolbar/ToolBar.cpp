#include <imgui.h>

#include "core/toolbar/ToolBar.h"
#include "core/Widget.h"

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
        auto playBtn = createWidget<Button>("Play");
        playBtn->getOnClickEvent().addListener([](){

        });
        playBtn->setEndOfLine(false);

        auto stopBtn = createWidget<Button>("Stop");        
        stopBtn->getOnClickEvent().addListener([](){

        });
        stopBtn->setEndOfLine(false);

        auto reloadBtn = createWidget<Button>("Reload");        
        reloadBtn->getOnClickEvent().addListener([](){

        });
    }

    void ToolBar::_drawImpl()
    {
        static bool initialized = false;
        if(!initialized)
        {
            initialize();
            initialized = true;
        }

        drawWidgets();
    }    
}
