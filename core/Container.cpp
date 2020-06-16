#include <algorithm>

#include "core/Container.h"
#include "core/Widget.h"

namespace ige::creator
{
    Container::Container() {}

    Container::~Container() 
    {
        removeAllWidgets();
    }

    void Container::addWidget(std::shared_ptr<Widget> widget)
    {
        widget->setContainer(this);
        m_widgets.push_back(widget);
    }

    void Container::removeWidget(std::shared_ptr<Widget> widget)
    {
        widget->setContainer(nullptr);

        auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&](auto itr)
        { 
            return itr == widget;
        });

        if (found != m_widgets.end())
        {
            m_widgets.erase(found);
        }
    }

    void Container::removeAllWidgets()
    {
        for (auto& widget : m_widgets)
            widget->setContainer(nullptr);
        m_widgets.clear();
    }

    void Container::drawWidgets()
    {
        std::for_each(m_widgets.begin(), m_widgets.end(), [](auto widget) {
            if(widget != nullptr) widget->draw();
        });
    }
}