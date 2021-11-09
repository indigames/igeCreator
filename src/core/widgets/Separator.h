#pragma once
#include "core/Widget.h"

namespace ige::creator
{
    class Separator: public Widget
    {
    protected:
        virtual void _drawImpl() override;
    };
}
