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

    void Container::addWidget(const std::shared_ptr<Widget>& widget)
    {
        widget->setContainer(this);
        m_widgets.push_back(widget);
    }

    void Container::removeWidget(const std::shared_ptr<Widget>& widget)
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

    void Container::removeWidgetById(const std::string& widgetId)
    {
        auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&](std::shared_ptr<Widget> itr)
        {
            return itr->getIdAString() == widgetId;
        });

        if (found != m_widgets.end())
        {
            (*found)->setContainer(nullptr);
            m_widgets.erase(found);
        }
    }

    void Container::removeWidgetById(uint64_t widgetId)
    {
        auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&](std::shared_ptr<Widget> itr)
        {
            return itr->getId() == widgetId;
        });

        if (found != m_widgets.end())
        {
            (*found)->setContainer(nullptr);
            m_widgets.erase(found);
        }
    }

    void Container::removeAllWidgets()
    {
        for (auto& widget : m_widgets)
        {
            if (widget != nullptr)
            {
                widget->setContainer(nullptr);
                widget = nullptr;
            }
        }
        m_widgets.clear();
    }

    void Container::drawWidgets()
    {
        for(const auto& widget: m_widgets) {
            if(widget != nullptr) widget->draw();
        }
    }
}