#include "core/widgets/AnchorWidget.h"
#include "core/layout/Columns.h"

namespace ige::creator
{
    AnchorWidget::AnchorWidget(const ImVec2& min, const ImVec2& max, bool eol)
        : Widget(true, eol), m_anchorMin(min), m_anchorMax(max)
    {
    }

    void AnchorWidget::_drawImpl()
    {
        ImGui::BeginChild(m_id.c_str(), ImVec2(42.f, 42.f), true, ImGuiWindowFlags_NoScrollbar);

        auto windowPos = ImGui::GetWindowPos();
        auto minPos = ImVec2(windowPos.x + 5.f, windowPos.y + 5.f);
        auto maxPos = ImVec2(minPos.x + 32.f, minPos.y + 32.f);
        auto midPos = ImVec2((maxPos.x + minPos.x) / 2.f - 0.5f, (maxPos.y + minPos.y) / 2.f - 0.5f);

        auto drawList = ImGui::GetWindowDrawList();
        drawList->AddRect(minPos, maxPos, ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 1.f)), false, ImDrawCornerFlags_None, 2.f);

        auto minRectPos = ImVec2(minPos.x + 8.f, minPos.y + 8.f);
        auto maxRectPos = ImVec2(maxPos.x - 8.f, maxPos.y - 8.f);
        drawList->AddRect(minRectPos, maxRectPos, ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 1.f)), false, ImDrawCornerFlags_None, 2.f);

        auto color = ImGui::GetColorU32(ImVec4(1.f, 0.f, 0.f, 1.0f));
        auto stretchColor = ImGui::GetColorU32(ImVec4(0.f, 0.f, 1.f, 1.0f));

        auto isStretchW = (m_anchorMin.x == 0.f && m_anchorMax.x == 1.f);
        auto isStretchH = (m_anchorMin.y == 0.f && m_anchorMax.y == 1.f);
        
        // Now, just consider 09 fixed anchor points
        auto anchorX = (m_anchorMin.x + m_anchorMax.x) / 2.f;
        auto anchorY = (m_anchorMin.y + m_anchorMax.y) / 2.f;

        auto curX = minPos.x + (maxPos.x - minPos.x) * anchorX;
        auto curY = maxPos.y - (maxPos.y - minPos.y) * anchorY;

        // Draw anchor indicator first    
        if(!isStretchW)
            drawList->AddRect(ImVec2(curX, minPos.y), ImVec2(curX, maxPos.y), color, false, ImDrawCornerFlags_None, 1.f);
        if (!isStretchH)
            drawList->AddRect(ImVec2(minPos.x, curY), ImVec2(maxPos.x, curY), color, false, ImDrawCornerFlags_None, 1.f);

        // Draw intersected point as circle
        drawList->AddCircleFilled(ImVec2(curX, curY), 2.5f, color, 16);

        // Then draw stretch indicators
        if (isStretchW)
            drawList->AddLine(ImVec2(minRectPos.x, midPos.y), ImVec2(maxRectPos.x, midPos.y), stretchColor, 3.f);
        if (isStretchH)
            drawList->AddLine(ImVec2(midPos.x, minRectPos.y), ImVec2(midPos.x, maxRectPos.y), stretchColor, 3.f);
                
        ImGui::EndChild();

        if(ImGui::IsItemClicked()) 
            getOnClickEvent().invoke(this);
    }


    AnchorPresets::AnchorPresets(const std::string& label)
        : Widget(), m_label(label)
    {
        for(int y = 2; y >= 0; y--)
        {
            for(int x = 0; x < 3; x++)
            {
                createWidget<AnchorWidget>(ImVec2(x / 2.f, y / 2.f), ImVec2(x / 2.f, y / 2.f), false);
                if (x == 2)
                {
                    createWidget<AnchorWidget>(ImVec2(0.f, y / 2.f), ImVec2(1.f, y / 2.f));
                }                
            }
        }
        for (int x = 0; x < 3; x++)
        {
            createWidget<AnchorWidget>(ImVec2(x / 2.f, 0.f), ImVec2(x / 2.f, 1.f), false);
            if (x == 2)
            {
                createWidget<AnchorWidget>(ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));
            }
        }

        for (auto& widget : m_widgets)
        {
            widget->getOnClickEvent().addListener([this](auto widget) {
                getOnClickEvent().invoke(widget);
            });
        }
    } 

    void AnchorPresets::_drawImpl()
    {
        if (ImGui::BeginPopup(m_label.c_str()))
        { 
            for (auto widget : m_widgets)
            {
                widget->draw();
            }
            ImGui::EndPopup();
        }
    }
}
