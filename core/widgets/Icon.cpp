#include "core/widgets/Icon.h"

namespace ige::creator
{
    Icon::Icon(const std::string& label, uint32_t textureId, const ImVec2& size, bool enable)
        : Widget(enable), m_label(label), m_textureId(textureId), m_size(size)
    {}

    Icon::~Icon()
    {}

    void Icon::_drawImpl()
    {
        if(ImGui::ImageButton((ImTextureID)m_textureId, m_size, {0.f, 1.f}, {1.f, 0.f}))
        {
            getOnClickEvent().invoke();
        }

        bool dummy;
        if (ImGui::Selectable((m_label + m_id).c_str(), &dummy))
        {
            getOnClickEvent().invoke();
        }
    }
}
