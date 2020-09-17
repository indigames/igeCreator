#pragma once

#include "core/Widget.h"
#include "core/widgets/Slider.h"

namespace ige::creator
{
    template <typename T, size_t N>
    Slider<T, N>::Slider(const std::string& label, ImGuiDataType type, const std::array<T, N>& val, const T& min, const T& max)
        : DataWidget(val), m_label(label), m_dataType(type), m_min(min), m_max(max)
    {
    }

    template <typename T, size_t N>
    Slider<T, N>::~Slider()
    {        
    }

    template <typename T, size_t N>
    void Slider<T, N>::_drawImpl()
    {
        if (m_max < m_min)
            m_max = m_min;
        
        if (m_data < m_min)
            m_data = m_min;
        else if (m_data > m_max)
            m_data = m_max;

        if (ImGui::SliderScalar((m_label + m_id).c_str(), m_dataType, &m_data, &m_min, &m_max))
        {
            notifyChange(m_data);
        }
    }
}
