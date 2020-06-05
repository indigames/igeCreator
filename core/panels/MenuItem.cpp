#include <imgui.h>

#include "core/panels/MenuItem.h"

namespace ige::creator
{
    MenuItem::MenuItem(const std::string& name, const std::string& shortcut)
        : m_name(name), m_shortcut(shortcut)
    {}

    MenuItem::~MenuItem()
    {}

    void MenuItem::_drawImpl()
    {
        if (ImGui::MenuItem((m_name).c_str(), m_shortcut.c_str(), nullptr, m_bIsEnable))
        {
            m_clickEvent.invoke();
        }		
    }
}