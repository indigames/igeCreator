#pragma once

#include <string>
#include <array>

#include "core/Container.h"
#include "core/Widget.h"

namespace ige::creator
{
    template <size_t N>
    class Columns: public Widget, public Container
    {
    public:
        Columns(float defautWidth = -1.f, bool isChild = false, float childTotalHeight = 64.f, bool enable = true);
        virtual ~Columns() {}

        void setColumnWidth(int idx, float width);   

    protected:
        virtual void _drawImpl() override;

        std::array<float, N> m_columnWidths;
        float m_defaultWidth = -1.f;
        float m_regionWidth = -1.f;

        // Set to true if this is a column inside other column
        // https://github.com/ocornut/imgui/issues/1028
        bool m_bIsChild;

        // Set total height of child columns group
        // https://github.com/ocornut/imgui/issues/1395
        float m_childTotalHeight;
    };

    template <size_t N>
    Columns<N>::Columns(float defautWidth, bool isChild, float childTotalHeight, bool enable)
        : Widget(enable), m_bIsChild(isChild), m_childTotalHeight(childTotalHeight)
    {
        m_columnWidths.fill(defautWidth);
    }

    template <size_t N>
    void Columns<N>::setColumnWidth(int idx, float width)
    {
        if(idx >= 0 && idx < N)
        {
            m_columnWidths[idx] = width;
        }
    }

    template <size_t N>
    void Columns<N>::_drawImpl()
    {
        if (m_regionWidth != ImGui::GetWindowContentRegionWidth()) {
            int devider = 0;
            float deductor = 0.f;
            for (int i = 0; i < N; ++i) {
                if (m_columnWidths[i] <= 0.f)
                    devider++;
                else
                    deductor += m_columnWidths[i];
            }
            m_defaultWidth = devider > 0 ? (ImGui::GetWindowContentRegionWidth() - deductor) / devider : 0.f;
            m_regionWidth = ImGui::GetWindowContentRegionWidth();
        }

        if (m_bIsChild) ImGui::BeginChild(("##" + getIdAsString()).c_str(), ImVec2(0.f, m_childTotalHeight));
        {
            ImGui::Columns(static_cast<int>(N), ("##" + getIdAsString()).c_str(), false);

            int counter = 0;
            int offset = 0;
            for (auto widget : m_widgets)
            {
                if (m_columnWidths[counter] > 0.f)
                {
                    ImGui::SetColumnOffset(counter, offset);
                    ImGui::SetColumnWidth(counter, m_columnWidths[counter]);
                    offset += m_columnWidths[counter];
                }
                else
                {
                    ImGui::SetColumnOffset(counter, offset);
                    ImGui::SetColumnWidth(counter, m_defaultWidth);
                    offset += m_defaultWidth;
                }
                widget->draw();

                ImGui::NextColumn();
                ++counter;
                if (counter == N) {
                    counter = 0;
                    offset = 0;
                }
            }
            ImGui::Columns(1);
        }
        if (m_bIsChild) ImGui::EndChild();
    }
}
