#pragma once

#include "core/Widget.h"
#include "core/widgets/Drag.h"

#include <imgui.h>
#include <imgui_internal.h>

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
        : DataWidget<std::array<T, N>>(val), m_label(label), m_dataType(type), m_speed(speed), m_min(minVal), m_max(maxVal), m_dragging(false)
    {
        m_label.append(this->getIdAsString());
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
            if (this->m_data[i] < m_min)
                this->m_data[i] = m_min;
            else if (this->m_data[i] > m_max)
                this->m_data[i] = m_max;
        }

        auto label = m_label.c_str();
        void* p_data = this->m_data.data();
        int changedIdx = -1;

        auto& g = *(ImGui::GetCurrentContext());
        ImGui::BeginGroup();
        ImGui::PushID(label);
        ImGui::PushMultiItemsWidths(N, ImGui::CalcItemWidth());
        auto type_size = ImGui::DataTypeInfo[m_dataType].Size;
        bool begin = false;
        bool finish = false;
        for (int i = 0; i < N; i++)
        {
            ImGui::PushID(i);
            if (i > 0)
                ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);

            // Notice: to show NAN the data type must be Float, so to present Int type we use %.0f format here
            if (ImGui::DragScalar("", ImGuiDataType_Float, p_data, m_speed, &m_min, &m_max, m_dataType == ImGuiDataType_Float ? "%.4f" : "%.0f", ImGuiSliderFlags_ClampOnInput)) {
                changedIdx = i;
                if (!m_dragging) begin = true;
                m_dragging = true;
            }
            else if (m_dragging) {
                if (ImGui::IsMouseReleased(0)) {
                    m_dragging = false;
                    finish = true;
                }
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
            if (begin) {
                notifyBeginChange(this->m_data);
            }
            if (std::isnan((float)this->m_data[changedIdx]))
                this->m_data[changedIdx] = 0;
            this->notifyChange(this->m_data);
        }
        if (finish) {
            notifyFinishChange(this->m_data);
        }
    }

    template <typename T, size_t N>
    void Drag<T, N>::notifyFinishChange(const std::array<T, N>& data) {
        m_onDataFinishChangedEvent.invoke(data);
    }

    template <typename T, size_t N>
    void Drag<T, N>::notifyBeginChange(const std::array<T, N>& data) {
        m_onDataBeginChangedEvent.invoke(data);
    }
}
