#include <imgui.h>

#include "core/toolbar/ToolBar.h"

#include "core/Widget.h"
#include "core/widgets/Button.h"
#include "core/widgets/TextField.h"
#include "core/widgets/Slider.h"

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
        createWidget<Button>("Play", ImVec2(32.f, 32.f), true, false)->getOnClickEvent().addListener([](){
            // TODO
        });        

        createWidget<Button>("Stop", ImVec2(32.f, 32.f), true, false)->getOnClickEvent().addListener([](){
            // TODO
        });        

        createWidget<Button>("Reload", ImVec2(32.f, 32.f), true, true)->getOnClickEvent().addListener([](){

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
