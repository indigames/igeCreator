#include <imgui.h>

#include "core/menu/MenuBar.h"
#include "core/menu/Menu.h"
#include "core/menu/MenuItem.h"
#include "core/widgets/Button.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/dialog/OpenFileDialog.h"
#include "core/panels/Hierarchy.h"

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
        fileMenu->createWidget<MenuItem>("Open Scene", "CTRL + O")->getOnClickEvent().addListener([](){
            auto selectedFiles = OpenFileDialog("Open", ".", {"json", "*.json"}).result();
            if (!selectedFiles.empty() && !selectedFiles[0].empty())
            {
                Editor::getCanvas()->getHierarchy()->clear();
                Editor::getSceneManager()->loadScene(selectedFiles[0]);
            }
        });

        fileMenu->createWidget<MenuItem>("Exit", "CTRL + N")->getOnClickEvent().addListener([]() {
            Editor::getApp()->quit();
        });
    }
}
