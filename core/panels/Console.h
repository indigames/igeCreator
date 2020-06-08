#pragma once

#include "core/Panel.h"

namespace ige::creator
{
    class Console: public Panel
    {
    public:
        Console(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Console();

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;
    };
}
