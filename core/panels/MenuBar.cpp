#include <imgui.h>

#include "core/panels/Menu.h"
#include "core/panels/MenuBar.h"
#include "core/panels/MenuItem.h"

namespace ige::creator
{
    MenuBar::MenuBar(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
    }
    
    MenuBar::~MenuBar()
    {

    }

    void MenuBar::initialize()
    {
        createFileMenu();
    }

    void MenuBar::_drawImpl()
    {
        if (m_widgets.empty()) return;

        if(ImGui::BeginMainMenuBar())
        {
            drawWidgets();
            ImGui::EndMainMenuBar();
        }
    }

    void MenuBar::createFileMenu()
    {
        auto fileMenu = createWidget<Menu>("File");
        fileMenu->createWidget<MenuItem>("New Scene", "CTRL + N")->getClickedEvent().addListener([](){
            ImGui::LogText("New Scene clicked");
        });

        fileMenu->createWidget<MenuItem>("Exit", "CTRL + N")->getClickedEvent().addListener([]() {
            exit(0);
        });        
    }
}
