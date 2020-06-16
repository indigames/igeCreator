#pragma once

#include <string>
#include "core/Container.h"
#include "core/Widget.h"

namespace ige::creator
{
    template <size_t N>
    class Columns: public Widget, public Container
    {
    public:
        Columns(float defautWidth = 180.f, bool enable = true);
        virtual ~Columns() {}

        void setColumnWidth(int idx, float width);   

    protected:
        virtual void _drawImpl() override;

        std::array<float, N> m_columnWidths;
    };

    template <size_t N>
    Columns<N>::Columns(float defautWidth, bool enable)
        : Widget(enable)
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
        ImGui::Columns(static_cast<int>(N), ("##" + m_id).c_str(), false);

        int counter = 0;
        for (auto widget : m_widgets)
        {           
            if (m_columnWidths[counter] > 0.f)
                ImGui::SetColumnWidth(counter, m_columnWidths[counter]);

            widget->draw();

            ImGui::NextColumn();
            
            ++counter;
            if (counter == N)
                counter = 0;
        }
        ImGui::Columns(1);
    }
}
