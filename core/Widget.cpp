#include "core/Widget.h"

namespace ige::creator
{
    uint64_t Widget::s_idCounter = 0;

    Widget::Widget()
    {
        m_id = "#" + std::to_string(s_idCounter++);
    }

    Widget::~Widget()
    {

    }

    void Widget::draw()
    {
        if(isEnable())
        {
            _drawImpl();
        }
    }
}
