#include "core/widgets/Separator.h"

#include <imgui.h>

namespace ige::creator
{
    void Separator::_drawImpl()
    {
        ImGui::Separator();
    }
}
