#pragma once

#include <imgui.h>

#include <string>
#include "core/plugin/IPlugin.h"

#include <event/Event.h>

namespace ige::creator
{
    enum class EDragDropID
    {
       FILE = 0,
    };

    // Drag & Drop source plugin
    template<typename T>
    class DDSourcePlugin: public IPlugin
    {
    public:
        DDSourcePlugin(EDragDropID id, const std::string& tooltip, const T& data);
        DDSourcePlugin(std::string id, const std::string& tooltip, const T& data);
        virtual ~DDSourcePlugin() {}

        virtual void execute();

        bool isDragged() const
        {
            return m_isDragged;
        }

        const std::string& getId() { return m_id; }

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

    // Drag & Drop target plugin
    template<typename T>
    class DDTargetPlugin: public IPlugin
    {
    public:
        DDTargetPlugin(EDragDropID id);
        DDTargetPlugin(std::string id);
        virtual ~DDTargetPlugin() {}

        virtual void execute();

        ige::scene::Event<>& getOnStartHoverEvent() { return m_onStartHoverEvent; }
        ige::scene::Event<>& getOnStopHoverEvent() { return m_onStopHoverEvent; }
        ige::scene::Event<T&>& getOnDataReceivedEvent() { return m_onDataReceivedEvent; }

        bool isHovered() const
        {
            return m_isHovered;
        }
        
        const std::string& getId() { return m_id; }

    protected:
        std::string m_id;
       
        ige::scene::Event<> m_onStartHoverEvent;
        ige::scene::Event<> m_onStopHoverEvent;
        ige::scene::Event<T&> m_onDataReceivedEvent;

        bool m_isHovered = false;
    };
    
    // DDSourcePlugin implementation
    template<typename T>
    DDSourcePlugin<T>::DDSourcePlugin(EDragDropID id, const std::string& tooltip, const T& data)
        : m_tooltip(tooltip), m_data(data)
    {
        m_id = std::to_string((int)id);
    }

    template<typename T>
    DDSourcePlugin<T>::DDSourcePlugin(std::string id, const std::string& tooltip, const T& data)
        : m_tooltip(tooltip), m_data(data)
    {
        m_id = id;
    }

    template<typename T>
    void DDSourcePlugin<T>::execute()
    {
        ImGuiDragDropFlags flags = 0;
        flags |= ImGuiDragDropFlags_SourceNoDisableHover;
        flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
        flags |= ImGuiDragDropFlags_SourceAllowNullID;

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

    // DDTargetPlugin implementation
    template<typename T>
    DDTargetPlugin<T>::DDTargetPlugin(EDragDropID id)
    {
        m_id = std::to_string((int)id);
    }

    template<typename T>
    DDTargetPlugin<T>::DDTargetPlugin(std::string id)
    {
        m_id = id;
    }

    template<typename T>
    void DDTargetPlugin<T>::execute()
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (!m_isHovered)
                m_onStartHoverEvent.invoke();

            m_isHovered = true;

            ImGuiDragDropFlags flags = 0;
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(getId().c_str(), flags))
            {
                T data = *(T*)payload->Data;
                m_onDataReceivedEvent.invoke(data);
            }
            ImGui::EndDragDropTarget();
        }
        else
        {
            if (m_isHovered)
                m_onStopHoverEvent.invoke();

            m_isHovered = false;
        }
    }
}
