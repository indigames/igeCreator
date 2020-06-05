#include <imgui.h>

#include "core/menu/Menu.h"

namespace ige::creator
{
    Menu::Menu(const std::string& name, bool enable)
        : Widget(enable), m_name(name)
    {}

    Menu::~Menu() {}

    void Menu::_drawImpl()
    {
        if (ImGui::BeginMenu(m_name.c_str(), m_bEnabled))
        {
            if (!m_bIsOpened)
            {
                m_onClickEvent.invoke();
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