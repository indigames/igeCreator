#pragma once

#include "core/Panel.h"

namespace ige::creator
{
    class AssetViewer: public Panel
    {
    public:
        AssetViewer(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~AssetViewer();

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;
    };
}
