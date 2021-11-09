#include "core/Widget.h"

#include <imgui.h>

namespace ige::creator
{
    uint64_t Widget::s_idCounter = 0;

    Widget::Widget(bool enable, bool eol)
        : IDrawable(), Pluggable(), m_bEnabled(enable), m_bEOL(eol), m_container(nullptr)
    {
        m_id = s_idCounter++;
        m_bHovered = false;
    }

    Widget::~Widget()
    {
        m_container = nullptr;
        removeAllPlugins();
        getOnClickEvent().removeAllListeners();
        getOnHoveredEvent().removeAllListeners();
    }

    void Widget::draw()
    {
        if(isEnable())
        {
            _drawImpl();

            if (!m_bHovered && ImGui::IsItemHovered())
            {
                m_bHovered = true;
                getOnHoveredEvent().invoke(this);
            }
            else if (m_bHovered && !ImGui::IsItemHovered())
            {
                m_bHovered = false;
                getOnHoveredEvent().invoke(this);
            }

            // execute plugins
            executePlugins();

            if (!isEndOfLine())
            {
                ImGui::SameLine();
            }
        }
    }
}
