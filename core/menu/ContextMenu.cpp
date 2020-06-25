#include <imgui.h>

#include "core/menu/ContextMenu.h"

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
        if (ImGui::BeginPopupContextItem())
        {
            drawWidgets();
            ImGui::EndPopup();
        }
    }

    void ContextMenu::close()
    {
        ImGui::CloseCurrentPopup();
    }
}