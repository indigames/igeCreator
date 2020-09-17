#pragma once
#include "core/Widget.h"
#include "core/Container.h"

#include <imgui.h>

namespace ige::creator
{
    class AnchorWidget : public Widget
    {
    public:
        AnchorWidget(const ImVec2& min, const ImVec2& max, bool eol = true);
        virtual ~AnchorWidget() {}

        const ImVec2& getAnchorMin() const { return m_anchorMin; }
        const ImVec2& getAnchorMax() const { return m_anchorMax; }

    protected:
        void _drawImpl() override;
        
        ImVec2 m_anchorMin;
        ImVec2 m_anchorMax;
    };

    class AnchorPresets: public Widget, public Container
    {
    public:
        AnchorPresets(const std::string& label);
        virtual ~AnchorPresets() {}

    protected:
        void _drawImpl() override;
        std::string m_label;
    };
}
