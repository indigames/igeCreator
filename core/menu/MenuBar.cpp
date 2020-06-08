#include <imgui.h>

#include "core/menu/Menu.h"
#include "core/menu/MenuBar.h"
#include "core/menu/MenuItem.h"
#include "core/button/Button.h"

namespace ige::creator
{
    MenuBar::MenuBar(const std::string& name, bool enable)
        : Widget(enable), m_name(name)
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
        static bool initialized = false;
        if (!initialized)
        {
            initialize();
            initialized = true;
        }

        if (m_widgets.empty()) return;

        if(ImGui::BeginMainMenuBar())
        {
            drawWidgets();

            float offset = ImGui::GetWindowHeight();
            ImGui::EndMainMenuBar();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset);
        }
    }

    void MenuBar::createFileMenu()
    {
        auto fileMenu = createWidget<Menu>("File");
        fileMenu->createWidget<MenuItem>("New Scene", "CTRL + N")->getOnClickEvent().addListener([](){
            ImGui::LogText("New Scene clicked");
        });

        fileMenu->createWidget<MenuItem>("Exit", "CTRL + N")->getOnClickEvent().addListener([]() {
            exit(0);
        });        
    }
}
