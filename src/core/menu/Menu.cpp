#include "core/menu/Menu.h"

#include <imgui.h>

namespace ige::creator
{
    Menu::Menu(const std::string& name, bool enable)
        : Widget(enable), m_name(name)
    {}

    Menu::~Menu() {}

    void Menu::_drawImpl()
    {
        if (ImGui::BeginMenu((m_name + getIdAsString()).c_str(), m_bEnabled))
        {
            if (!m_bIsOpened)
            {
                getOnClickEvent().invoke(this);
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