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

    void Container::addWidget(std::shared_ptr<Widget> widget, int pos)
    {
        if (widget == nullptr) return;
        auto found = std::find(m_widgets.begin(), m_widgets.end(), widget);
        if (found == m_widgets.end())
        {
            auto thisWidget = dynamic_cast<Widget*>(this);
            if (!thisWidget || thisWidget != widget.get())
            {
                widget->setContainer(this);
                if (pos >= 0 && pos < m_widgets.size()) {
                    m_widgets.insert(m_widgets.begin() + pos, widget);
                }
                else {
                    m_widgets.push_back(widget);
                }                
            }
        }
    }

    void Container::removeWidget(std::shared_ptr<Widget> widget)
    {
        if (widget == nullptr) return;
        auto found = std::find(m_widgets.begin(), m_widgets.end(), widget);
        if (found != m_widgets.end()) {
            m_widgets.erase(found);
        }
    }

    void Container::removeWidgetById(const std::string& widgetId)
    {
        auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&](std::shared_ptr<Widget> itr)
        {
            return itr->getIdAsString() == widgetId;
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
                auto& group = std::dynamic_pointer_cast<Group>(widget);
                if (group) group->removeAllWidgets();
                group = nullptr;
            }
        }
        m_widgets.clear();
    }
    
    void Container::drawWidgets()
    {
        for(const auto& widget: m_widgets)
        {
            if(widget != nullptr) widget->draw();
        }
    }
}