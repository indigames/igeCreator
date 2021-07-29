#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include "core/Widget.h"
#include "core/widgets/Drag.h"

namespace ImGui {
    const ImGuiDataTypeInfo DataTypeInfo[] =
    {
        { sizeof(char),             "%d",   "%d"    },  // ImGuiDataType_S8
        { sizeof(unsigned char),    "%u",   "%u"    },
        { sizeof(short),            "%d",   "%d"    },  // ImGuiDataType_S16
        { sizeof(unsigned short),   "%u",   "%u"    },
        { sizeof(int),              "%d",   "%d"    },  // ImGuiDataType_S32
        { sizeof(unsigned int),     "%u",   "%u"    },
    #ifdef _MSC_VER
        { sizeof(ImS64),            "%I64d","%I64d" },  // ImGuiDataType_S64
        { sizeof(ImU64),            "%I64u","%I64u" },
    #else
        { sizeof(ImS64),            "%lld", "%lld"  },  // ImGuiDataType_S64
        { sizeof(ImU64),            "%llu", "%llu"  },
    #endif
        { sizeof(float),            "%f",   "%f"    },  // ImGuiDataType_Float (float are promoted to double in va_arg)
        { sizeof(double),           "%f",   "%lf"   },  // ImGuiDataType_Double
    };
}

namespace ige::creator
{
    template <typename T, size_t N>
    Drag<T, N>::Drag(const std::string& label, ImGuiDataType type, const std::array<T, N>& val, float speed, const T& minVal, const T& maxVal)
        : DataWidget(val), m_label(label), m_dataType(type), m_speed(speed), m_min(minVal), m_max(maxVal)
    {
        m_label.append(getIdAString());
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

        auto label = m_label.c_str();
        void* p_data = m_data.data();
        int changedIdx = -1;

        ImGuiContext& g = *GImGui;
        ImGui::BeginGroup();
        ImGui::PushID(label);
        ImGui::PushMultiItemsWidths(N, ImGui::CalcItemWidth());
        auto type_size = ImGui::DataTypeInfo[m_dataType].Size;
        for (int i = 0; i < N; i++)
        {
            ImGui::PushID(i);
            if (i > 0)
                ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

            // Notice: to show NAN the data type must be Float, so to present Int type we use %.0f format here
            if (type_size == ImGui::DataTypeInfo[ImGuiDataType_Float].Size)
            {
                if(ImGui::DragScalar("", ImGuiDataType_Float, p_data, m_speed, &m_min, &m_max, m_dataType == ImGuiDataType_Float ? "%.3f" : "%.0f"))
                    changedIdx = i;
            }            
            else if(ImGui::DragScalar("", m_dataType, p_data, m_speed, &m_min, &m_max))
            {
                changedIdx = i;
            }

            ImGui::PopID();
            ImGui::PopItemWidth();
            p_data = (void*)((char*)p_data + type_size);
        }
        ImGui::PopID();

        const char* label_end = ImGui::FindRenderedTextEnd(label);
        if (label != label_end)
        {
            ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
            ImGui::TextEx(label, label_end);
        }

        ImGui::EndGroup();

        if (changedIdx != -1)
        {
            if (std::isnan((float)m_data[changedIdx]))
                m_data[changedIdx] = 0;
            notifyChange(m_data);
        }
    }
}
