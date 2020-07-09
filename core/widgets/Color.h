#pragma once

#include <imgui.h>

#include "core/Widget.h"

namespace ige::creator
{
    class Color: public Widget
    {
    public:
        Color(const std::string& label, float* value);
        virtual ~Color();

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        float* m_value;
    };
}
