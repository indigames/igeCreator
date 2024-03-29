#pragma once

#include "core/Widget.h"
#include "core/Container.h"
#include "core/widgets/Button.h"

namespace ige::creator
{
    class ToolBar: public Widget, public Container
    {
    public:
        ToolBar(const std::string& name = "", bool enable = true);
        virtual ~ToolBar();        

    protected:
        virtual void initialize();
        virtual void _drawImpl();

    protected:
        std::string m_name;
        std::shared_ptr<Button> m_playBtn = nullptr;
        std::shared_ptr<Button> m_stopBtn = nullptr;
    };
}