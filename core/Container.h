#pragma once

#include <vector>
#include <memory>

#include "core/IDrawable.h"
#include "core/Widget.h"

namespace ige::creator
{
    class Container
    {
    public:
        Container();
        virtual ~Container();
        
        template <typename T, typename... Args>
		std::shared_ptr<T> createWidget(Args&&... args)
		{
            auto widget = std::make_shared<T>(args...);
            widget->setContainer(this);
			addWidget(widget);
			return widget;
		}

        virtual void addWidget(const std::shared_ptr<Widget>& widget);

        virtual void removeWidget(const std::shared_ptr<Widget>& widget);

        virtual void removeWidgetById(const std::string& widgetId);

        virtual void removeAllWidgets();

        virtual void drawWidgets();

    protected:
        std::vector<std::shared_ptr<Widget>> m_widgets;
    };    
}