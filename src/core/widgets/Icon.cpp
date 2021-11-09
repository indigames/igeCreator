#include "core/widgets/Icon.h"
namespace ige::creator
{
    Icon::Icon(const std::string& label, uint32_t textureId, const ImVec2& size, bool enable, bool eol)
        : Widget(enable, eol), m_label(label), m_textureId(textureId), m_size(size)
    {}

    Icon::~Icon()
    {}

    void Icon::_drawImpl()
    {
        ImGui::PushID((m_label + getIdAsString()).c_str());
        ImGui::BeginGroup();
        {
            if (ImGui::ImageButton((ImTextureID)m_textureId, m_size, { 0.f, 1.f }, { 1.f, 0.f }))
            {
                getOnClickEvent().invoke(this);
            }
            if (ImGui::Selectable((m_label + getIdAsString()).c_str(), false, 0, ImVec2(128.f, 0.f)))
            {
                getOnClickEvent().invoke(this);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip(m_label.c_str());
            }
        }
        ImGui::EndGroup();
        ImGui::PopID();
    }
}
