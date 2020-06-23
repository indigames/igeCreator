#pragma once

#include <imgui.h>

#include <string>
#include "core/plugin/IPlugin.h"

namespace ige::creator
{
    template<typename T>
    class DDSourcePlugin: public IPlugin
    {
    public:
        DDSourcePlugin(const std::string& id, const std::string& tooltip, const T& data);
        ~DDSourcePlugin();

        virtual void execute();

        bool isDragged() const
        {
            return m_isDragged;
        }

        ige::scene::Event<>& getOnStartDragEvent() { return m_onStartDragEvent; }
        ige::scene::Event<>& getOnStopDragEvent() { return m_onStopDragEvent; }

    protected:
        std::string m_id;
        std::string m_tooltip;
        T m_data;
        ige::scene::Event<> m_onStartDragEvent;
        ige::scene::Event<> m_onStopDragEvent;

        bool m_bHasTooltip = true;
        bool m_isDragged = false;
    };

    template<typename T>
    DDSourcePlugin<T>::DDSourcePlugin(const std::string& id, const std::string& tooltip, const T& data)
        : m_id(id), m_tooltip(tooltip), m_data(data)
    {

    }

    template<typename T>
    DDSourcePlugin<T>::~DDSourcePlugin()
    {

    }

    template<typename T>
    void DDSourcePlugin<T>::execute()
    {
        ImGuiDragDropFlags flags = 0;
        flags |= ImGuiDragDropFlags_SourceNoDisableHover;
        flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;

        if (!m_bHasTooltip)
            flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip;

        if (ImGui::BeginDragDropSource(flags))
        {
            if (!m_isDragged)
                m_onStartDragEvent.invoke();

            m_isDragged = true;

            if (!(flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                ImGui::Text(m_tooltip.c_str());

            ImGui::SetDragDropPayload(m_id.c_str(), &m_data, sizeof(m_data));
            ImGui::EndDragDropSource();
        }
        else
        {
            if (m_isDragged)
                m_onStopDragEvent.invoke();

            m_isDragged = false;
        }
    }
}
