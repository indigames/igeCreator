#pragma once

#include <imgui.h>

#include "core/Widget.h"
#include "core/widgets/Drag.h"

namespace ige::creator
{
    template <typename T, size_t N>
    Drag<T, N>::Drag(const std::string& label, ImGuiDataType type, const std::array<T, N>& val, float speed, const T& minVal, const T& maxVal)
        : DataWidget(val), m_label(label), m_dataType(type), m_speed(speed), m_min(minVal), m_max(maxVal)
    {
    }

    template <typename T, size_t N>
    Drag<T, N>::~Drag()
    {        
    }

    template <typename T, size_t N>
    void Drag<T, N>::_drawImpl()
    {
        if (m_max < m_min)
            m_max = m_min;
        
        for (size_t i = 0; i < N; ++i)
        {
            if (m_data[i] < m_min)
                m_data[i] = m_min;
            else if (m_data[i] > m_max)
                m_data[i] = m_max;
        }

        if (ImGui::DragScalarN((m_label + m_id).c_str(), m_dataType, m_data.data(), (int)N, m_speed, &m_min, &m_max, "%.3f"))
        {
            notifyChange(m_data);
        }
    }
}
