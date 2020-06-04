#pragma once

#include <vector>
#include <memory>

#include "core/IDrawable.h"

namespace ige::creator
{
    class Widget;

    class Container: std::enable_shared_from_this<Container>
    {
    public:
        Container();
        virtual ~Container();
        
        template <typename T, typename... Args>
		std::shared_ptr<T> createWidget(Args&&... args)
		{
            auto widget = std::make_shared<T>(args...);
            widget->setComponent(shared_from_this());
			addWidget(widget);
			return widget;
		}

        virtual void addWidget(std::shared_ptr<Widget> widget);

        virtual void removeWidget(std::shared_ptr<Widget> widget);

        virtual void removeAllWidgets();

        virtual void drawWidgets();

    protected:
        std::vector<std::shared_ptr<Widget>> m_widgets;
    };    
}