#pragma once

#include <imgui.h>

#include "core/Widget.h"

namespace ige::creator
{
    template <typename T>
    class Slider: public DataWidget<T>
    {
        static_assert(std::is_scalar<T>::value, "Invalid Slider<T>, scalar type expected!");

    public:
        Slider(const std::string& label, ImGuiDataType type, const T& min, const T& max, const T& val);
        virtual ~Slider();


    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        ImGuiDataType m_dataType;
        T m_min;
        T m_max;        
    };
}

#include "core/widgets/Slider.hpp"
