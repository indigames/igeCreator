#include <imgui.h>

#include "core/toolbar/ToolBar.h"
#include "core/Panel.h"

namespace ige::creator
{
    ToolBar::ToolBar(const std::string& name, const Settings& setting)
        : Panel(name, setting)
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
        Panel::_drawImpl();
    }    
}
