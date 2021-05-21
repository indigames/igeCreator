#include <imgui.h>
#include "core/Widget.h"

namespace ige::creator
{
    uint64_t Widget::s_idCounter = 0;

    Widget::Widget(bool enable, bool eol)
        : m_bEnabled(enable), m_bEOL(eol), m_container(nullptr)
    {
        m_id = s_idCounter++;
    }

    Widget::~Widget()
    {
        removeAllPlugins();
        getOnClickEvent().removeAllListeners();
    }

    void Widget::draw()
    {
        if(isEnable())
        {
            _drawImpl();

            // execute plugins
            executePlugins();

            if (!isEndOfLine())
            {
                ImGui::SameLine();
            }
        }
    }
}
