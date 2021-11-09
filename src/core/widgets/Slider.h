#pragma once
#include "core/Widget.h"

#include <imgui.h>
#include <limits>

namespace ige::creator
{
    template <typename T, size_t N = 1>
    class Slider: public DataWidget<T>
    {
        static_assert(std::is_scalar<T>::value, "Invalid Slider<T>, scalar type expected!");

    public:
        Slider(const std::string& label, ImGuiDataType type, const std::array<T, N>& val, const T& min = std::numeric_limits<T>::lowest(), const T& max = std::numeric_limits<T>::max());
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
