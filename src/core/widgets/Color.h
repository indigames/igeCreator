#pragma once
#include "core/Widget.h"

#include <utils/PyxieHeaders.h>
using namespace pyxie;

namespace ige::creator
{
    class Color: public DataWidget<Vec4>
    {
    public:
        Color(const std::string& label, const Vec4& value);
        virtual ~Color();

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
    };
}
