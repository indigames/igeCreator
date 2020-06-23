#include <imgui.h>
#include "core/Widget.h"

namespace ige::creator
{
    uint64_t Widget::s_idCounter = 0;

    Widget::Widget(bool enable)
        : m_bEnabled(enable)
    {
        m_id = "##" + std::to_string(s_idCounter++);
    }

    Widget::~Widget()
    {
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
