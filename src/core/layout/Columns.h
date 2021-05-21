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
        if (m_bIsChild) ImGui::BeginChild(("##" + getIdAString()).c_str(), ImVec2(0.f, m_childTotalHeight));
        {
            ImGui::Columns(static_cast<int>(N), ("##" + getIdAString()).c_str(), false);

            int counter = 0;
            int offset = 0;
            int fixedWidthCounter = 0;
            int fixedWidthOffset = 0;
            for (auto widget : m_widgets)
            {
                if (m_columnWidths[counter] > 0.f)
                {
                    ImGui::SetColumnWidth(counter, m_columnWidths[counter]);
                    offset += m_columnWidths[counter];
                    fixedWidthCounter++;
                    fixedWidthOffset = offset;
                }
                else
                {
                    auto width = (ImGui::GetWindowContentRegionWidth() - fixedWidthOffset) / (N - fixedWidthCounter);
                    ImGui::SetColumnOffset(counter, offset);
                    ImGui::SetColumnWidth(counter, width);
                    offset += width;
                }
                widget->draw();

                ImGui::NextColumn();
                ++counter;
                if (counter == N)
                {
                    counter = 0;
                    offset = 0;
                }
            }
            ImGui::Columns(1);
        }
        if (m_bIsChild) ImGui::EndChild();
    }
}
