#include "core/layout/Group.h"

#include <imgui.h>

namespace ige::creator
{
    Group::Group(const std::string& name, bool collapsable, bool closable, E_Align align, bool open, bool enable)
        : Container(), Widget(enable), m_name(name), m_bIsCollapsable(collapsable), m_bIsClosable(closable), m_align(align), m_bIsOpened(open)
    {
    }

    Group::~Group()
    {
        setContainer(nullptr);
        removeAllWidgets();
        getOnOpenedEvent().removeAllListeners();
        getOnClosedEvent().removeAllListeners();
    }

    void Group::_drawImpl()
    {
        auto width = ImGui::CalcItemWidth();
        switch (m_align)
        {
        case ige::creator::Group::E_Align::LEFT:
            break;
        case ige::creator::Group::E_Align::CENTER:
            ImGui::SameLine(width / 2.f);
            break;
        case ige::creator::Group::E_Align::RIGHT:
            ImGui::SameLine(width);
            break;
        default:
            break;
        }

        ImGui::BeginGroup();
        {
            bool prevOpened = m_bIsOpened;

            if (!m_bIsCollapsable || (m_bIsCollapsable && ImGui::CollapsingHeader((m_name + getIdAsString()).c_str(), m_bIsClosable ? &m_bIsOpened : nullptr, m_bIsOpened ? ImGuiTreeNodeFlags_DefaultOpen : 0)))
            {
                drawWidgets();
            }

            if (m_bIsOpened != prevOpened)
            {
                if (m_bIsOpened)
                    getOnOpenedEvent().invoke();
                else
                    getOnClosedEvent().invoke();
            }
        }        
        ImGui::EndGroup();
    }
}
