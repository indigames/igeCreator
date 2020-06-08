#pragma once

#include "core/Panel.h"

namespace ige::creator
{
    class Inspector: public Panel
    {
    public:
        Inspector(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Inspector();

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;
    };
}