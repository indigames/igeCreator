#include <imgui.h>

#include "core/layout/Group.h"

namespace ige::creator
{
    Group::Group(const std::string& name, bool collapsable, bool closable, bool open, bool enable)
        : Widget(enable), m_name(name), m_bIsCollapsable(collapsable), m_bIsOpened(open), m_bIsClosable(closable)
    {
    }

    Group::~Group()
    {
        getOnOpenedEvent().removeAllListeners();
        getOnClosedEvent().removeAllListeners();        
    }

    void Group::_drawImpl()
    {
        bool prevOpened = m_bIsOpened;

        if (!m_bIsCollapsable || (m_bIsCollapsable && ImGui::CollapsingHeader(m_name.c_str(), m_bIsClosable ? &m_bIsOpened : nullptr, m_bIsOpened ? ImGuiTreeNodeFlags_DefaultOpen : 0)))
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
}
