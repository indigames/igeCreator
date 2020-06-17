#include <imgui.h>

#include "core/layout/Group.h"

namespace ige::creator
{
    Group::Group(const std::string& name, bool collapsable, bool closable, bool enable)
        : Widget(enable), m_name(name), m_bIsCollapsable(collapsable), m_bIsClosable(closable)
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

        if (!m_bIsCollapsable || (m_bIsCollapsable && ImGui::CollapsingHeader(m_name.c_str(), m_bIsClosable ? &m_bIsOpened : nullptr)))
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
