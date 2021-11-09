#pragma once
#include "core/Widget.h"

#include <imgui.h>
#include <limits>

namespace ige::creator
{
    template <typename T, size_t N = 1>
    class Drag: public DataWidget<std::array<T, N>>
    {
        static_assert(std::is_scalar<T>::value, "Invalid Drag<T>, scalar type expected!");

    public:
        Drag(const std::string& label, ImGuiDataType type, const std::array<T, N>& val, float speed = 0.05f, const T& minVal = std::numeric_limits<T>::lowest(), const T& maxVal = std::numeric_limits<T>::max());
        virtual ~Drag();

    protected:
        virtual void _drawImpl() override;

        std::string m_label;
        ImGuiDataType m_dataType;
        float m_speed;
        T m_min;
        T m_max;
    };
}

#include "core/widgets/Drag.hpp"
