#pragma once

#include <memory>
#include <string>

#include "core/IDrawable.h"

namespace ige::creator
{
    class Container;
    class Widget: public IDrawable
    {
    public:
        Widget();
        virtual ~Widget();

        void draw() override;
        
        std::string getId() const { return m_id; }

        void setEnable(bool enable) { m_bIsEnable = enable; }
        bool isEnable() const { return m_bIsEnable; }

        void setContainer(std::shared_ptr<Container> container) { m_container = container; }
        bool hasContainer() const { return m_container.lock() != nullptr; };
        std::shared_ptr<Container> getContainer() const { return m_container.lock(); }

    protected:
        virtual void _drawImpl() = 0;

    protected:
        std::string m_id = "";
        std::weak_ptr<Container> m_container;
        bool m_bIsEnable = true;

    private:
        static uint64_t s_idCounter;
    };    
}