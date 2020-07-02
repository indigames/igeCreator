#include <imgui.h>

#include "core/toolbar/ToolBar.h"

#include "core/Widget.h"
#include "core/widgets/Button.h"
#include "core/widgets/TextField.h"
#include "core/widgets/Slider.h"

#include "utils/PyxieHeaders.h"

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
        createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/play")->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](){
            // TODO
        });        

        createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/pause")->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](){
            // TODO
        });        

        createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/stop")->GetTextureHandle(), ImVec2(16.f, 16.f), true, true)->getOnClickEvent().addListener([](){

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
