#pragma once

#include <memory>
#include <string>

#include "core/IDrawable.h"
#include "core/plugin/IPlugin.h"

#include <event/Event.h>

namespace ige::creator
{
    class Container;
    class Widget: public IDrawable, public Pluggable
    {
    public:
        Widget(bool enable = true, bool eol = true);
        virtual ~Widget();

        void draw() override;
        
        std::string getId() const { return m_id; }

        //! Enable/disable widget
        void setEnable(bool enable) { m_bEnabled = enable; }
        bool isEnable() const { return m_bEnabled; }

        //! Should break line after render this widget
        void setEndOfLine(bool eol) { m_bEOL = eol; }
        bool isEndOfLine() const { return m_bEOL; }

        void setContainer(Container* container) { m_container = container; }
        bool hasContainer() const { return m_container != nullptr; };
        Container* getContainer() const { return m_container; }

        ige::scene::Event<Widget*>& getOnClickEvent() { return m_onClickEvent; }

    protected:
        virtual void _drawImpl() = 0;

    protected:
        std::string m_id = "";
        Container* m_container;
        bool m_bEnabled;
        bool m_bEOL = true;
        ige::scene::Event<Widget*> m_onClickEvent;

    private:
        static uint64_t s_idCounter;
    };

    //! Data widget: widget with data
    template <typename T>
    class DataWidget : public Widget
    {
    public:
        DataWidget(const T& data, bool enable = true, bool eol = true) : Widget(enable, eol), m_data(data) {};
        virtual ~DataWidget() { getOnDataChangedEvent().removeAllListeners(); }

        ige::scene::Event<const T&>& getOnDataChangedEvent() { return m_onDataChangedEvent; }

    protected:
        void notifyChange(const T& data);

        T m_data;
        ige::scene::Event<const T&> m_onDataChangedEvent;
    };

    template<typename T>
    inline void DataWidget<T>::notifyChange(const T& data)
    {
        m_onDataChangedEvent.invoke(data);
    }
}