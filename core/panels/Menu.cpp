#include <imgui.h>

#include "core/panels/Menu.h"

namespace ige::creator
{
    Menu::Menu(const std::string& name)
        : m_name(name)
    {}

    Menu::~Menu() {}

    void Menu::_drawImpl()
    {
        if (ImGui::BeginMenu(m_name.c_str(), m_bIsEnable))
        {
            if (!m_bIsOpened)
            {
                m_clickedEvent.invoke();
                m_bIsOpened = true;
            }

            drawWidgets();
            ImGui::EndMenu();
        }
        else
        {
            m_bIsOpened = false;
        }
    }
}