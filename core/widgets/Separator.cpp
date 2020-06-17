#include <imgui.h>

#include "core/widgets/Separator.h"

namespace ige::creator
{
    void Separator::_drawImpl()
    {
        ImGui::Separator();
    }
}
