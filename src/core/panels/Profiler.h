#pragma once

#include "core/Panel.h"
#include "core/widgets/Label.h"

namespace ige::creator
{
    class Profiler: public Panel
    {
    public:
        Profiler(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Profiler();

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

    protected:
        std::shared_ptr<Label> m_timeWidget = nullptr;
        std::shared_ptr<Label> m_fpsWidget = nullptr;
    };
}
