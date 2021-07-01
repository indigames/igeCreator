#include <algorithm>

#include "core/Container.h"
#include "core/Widget.h"
#include "layout/Group.h"

namespace ige::creator
{
    Container::Container() {}

    Container::~Container() 
    {
        removeAllWidgets();
    }

    void Container::addWidget(std::shared_ptr<Widget> widget)
    {
        auto found = std::find(m_widgets.begin(), m_widgets.end(), widget);
        if (found == m_widgets.end())
        {
            auto thisWidget = dynamic_cast<Widget*>(this);
            if (!thisWidget || thisWidget != widget.get())
            {
                widget->setContainer(this);
                m_widgets.push_back(widget);
            }
        }
    }

    void Container::removeWidget(std::shared_ptr<Widget> widget)
    {
        auto found = std::find(m_widgets.begin(), m_widgets.end(), widget);
        if (found != m_widgets.end())
        {
            widget->setContainer(nullptr);
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
            if (widget)
            {
                widget->setContainer(nullptr);
                widget->removeAllPlugins();
                auto& group = std::dynamic_pointer_cast<Group>(widget);
                if (group) group->removeAllWidgets();
                widget = nullptr;
            }
        }
        m_widgets.clear();
    }
    
    void Container::drawWidgets()
    {
        for(const auto& widget: m_widgets)
        {
            widget->draw();
        }
    }
}