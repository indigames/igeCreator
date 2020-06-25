#include <imgui.h>

#include "core/menu/MenuItem.h"

namespace ige::creator
{
    MenuItem::MenuItem(const std::string& name, const std::string& shortcut, bool enable)
        : Widget(enable), m_name(name), m_shortcut(shortcut)
    {}

    MenuItem::~MenuItem()
    {}

    void MenuItem::_drawImpl()
    {
        if (ImGui::MenuItem((m_name + m_id).c_str(), m_shortcut.c_str(), nullptr, m_bEnabled))
        {
            m_onClickEvent.invoke();
        }
    }
}
