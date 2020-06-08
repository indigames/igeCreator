#include <imgui.h>

#include "core/toolbar/ToolBar.h"
#include "core/Widget.h"
#include "core/input/TextInput.h"
#include "core/input/Slider.h"

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
        reloadBtn->setEndOfLine(false);

        std::string inputText = "";
        auto inputTxt = createWidget<TextInput>("Input", inputText);
        inputTxt->getOnDataChangedEvent().addListener([](auto text) 
        {            
            ImGui::LogText(text.c_str());
        });
        // inputTxt->setEndOfLine(false);

        int val = 50;
        auto valSlider = createWidget<Slider<int>>("Val", ImGuiDataType_S32, 0, 100, val);
        valSlider->getOnDataChangedEvent().addListener([](auto val) {
            ImGui::LogText("Val: %d", val);
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
