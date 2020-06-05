#pragma once

#include <unordered_map>

#include "core/Panel.h"

namespace ige::creator
{
    class Hierarchy: public Panel
    {
    public:
        Hierarchy(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Hierarchy();       

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;
    };
}