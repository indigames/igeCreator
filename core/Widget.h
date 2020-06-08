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

        //! Enable/disable widget
        void setEnable(bool enable) { m_bEnabled = enable; }
        bool isEnable() const { return m_bEnabled; }

        //! Should break line after render this widget
        void setEndOfLine(bool eol) { m_bEOL = eol; }
        bool isEndOfLine() const { return m_bEOL; }

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
        bool m_bEOL = true;
        Event<> m_onClickEvent;

    private:
        static uint64_t s_idCounter;        
    };

    //! Data widget: widget with data
    template <typename T>
    class DataWidget : public Widget
    {
    public:
        DataWidget(T& data, bool enable = true) : Widget(enable), m_data(data) {};		

        Event<T&>& getOnDataChangedEvent() { return m_onDataChangedEvent; }

    protected:
        virtual void _drawImpl() override;
        void notifyChange(T& data);

        T m_data;
        Event<T&> m_onDataChangedEvent;
    };

    template<typename T>
    inline void DataWidget<T>::_drawImpl()
    {
        // Widget::_drawImpl();
    }

    template<typename T>
    inline void DataWidget<T>::notifyChange(T& data)
    {
        m_onDataChangedEvent.invoke(data);
    }
}