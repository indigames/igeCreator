#include "core/widgets/Image.h"

namespace ige::creator
{
    Image::Image(uint32_t textureId, const ImVec2& size, bool center, bool enable, bool eol)
        : Widget(enable, eol), m_textureId(textureId), m_size(size), m_bCenter(center)
    {}

    Image::~Image()
    {}

    void Image::_drawImpl()
    {
        if (m_bCenter) {
            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - m_size.x) * 0.5f, (ImGui::GetWindowSize().y - m_size.y) * 0.5f));
        }
        ImGui::Image((ImTextureID)m_textureId, m_size, {0.f, 1.f}, {1.f, 0.f});
    }
}
