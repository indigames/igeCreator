#pragma once

#include <memory>
#include <string>

#include "core/IDrawable.h"
#include "event/Event.h"

namespace ige::creator
{
    class Container;
    class Widget: public IDrawable
    {
    public:
        Widget(bool enable = true);
        virtual ~Widget();

        void draw() override;
        
        std::string getId() const { return m_id; }

        void setEnable(bool enable) { m_bEnabled = enable; }
        bool isEnable() const { return m_bEnabled; }

        void setContainer(std::shared_ptr<Container> container) { m_container = container; }
        bool hasContainer() const { return m_container.lock() != nullptr; };
        std::shared_ptr<Container> getContainer() const { return m_container.lock(); }

        Event<>& getOnClickEvent() { return m_onClickEvent; }
    protected:
        virtual void _drawImpl() = 0;

    protected:
        std::string m_id = "";
        std::weak_ptr<Container> m_container;
        bool m_bEnabled;
        Event<> m_onClickEvent;

    private:
        static uint64_t s_idCounter;        
    };    
}