#include "core/widgets/Image.h"

namespace ige::creator
{
    Image::Image(uint32_t textureId, const ImVec2& size, bool enable)
        : Widget(enable), m_textureId(textureId), m_size(size)
    {}

    Image::~Image()
    {}

    void Image::_drawImpl()
    {
        if(isEnable())
        {
            ImGui::Image((ImTextureID)m_textureId, m_size, {0.f, 1.f}, {1.f, 0.f});
        }
    }
}
