#pragma once

#include "core/Widget.h"
#include "core/Panel.h"
#include "core/button/Button.h"

namespace ige::creator
{
    class ToolBar: public Panel
    {
    public:
        ToolBar(const std::string& name = "", const Settings& setting = {});
        virtual ~ToolBar();        

    protected:
        virtual void initialize();
        virtual void _drawImpl();
    };
}