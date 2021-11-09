#include "core/menu/ContextMenu.h"

#include <imgui.h>

namespace ige::creator
{

    ContextMenu::ContextMenu(const std::string& name, bool enable)
        : Menu(name, enable)
    {}

    ContextMenu::~ContextMenu()
    {}

    void ContextMenu::_drawImpl() 
    {
        // DO NOTHING
    }

    void ContextMenu::execute()
    {
        if (ImGui::BeginPopupContextItem(m_name.c_str()))
        {
            drawWidgets();
            ImGui::EndPopup();
        }
    }

    void ContextMenu::open()
    {
        ImGui::OpenPopup(m_name.c_str());
    }
    
    void ContextMenu::close()
    {
        ImGui::CloseCurrentPopup();
    }


    WindowContextMenu::WindowContextMenu(const std::string& name, bool enable)
        : ContextMenu(name, enable)
    {}


    void WindowContextMenu::execute()
    {
        if (ImGui::BeginPopupContextWindow(m_name.c_str()))
        {
            drawWidgets();
            ImGui::EndPopup();
        }
    }
}