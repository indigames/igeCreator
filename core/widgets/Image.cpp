#include "core/widgets/Image.h"

namespace ige::creator
{
    Image::Image(uint32_t textureId, const ImVec2& size, bool enable, bool eol)
        : Widget(enable, eol), m_textureId(textureId), m_size(size)
    {}

    Image::~Image()
    {}

    void Image::_drawImpl()
    {
        ImGui::Image((ImTextureID)m_textureId, m_size, {0.f, 1.f}, {1.f, 0.f});
    }
}
