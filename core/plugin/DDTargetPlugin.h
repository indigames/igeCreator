#pragma once

#include <imgui.h>

#include <string>
#include "core/plugin/IPlugin.h"

namespace ige::creator
{
    template<typename T>
    class DDTargetPlugin: public IPlugin
    {
    public:
        DDTargetPlugin(const std::string& id);
        ~DDTargetPlugin() {}

        virtual void execute();

        ige::scene::Event<>& getOnStartHoverEvent() { return m_onStartHoverEvent; }
        ige::scene::Event<>& getOnStopHoverEvent() { return m_onStopHoverEvent; }
        ige::scene::Event<>& getOnDataReceivedEvent() { return m_onDataReceivedEvent; }

        bool isHovered() const
        {
            return m_isHovered;
        }
        
    protected:
        std::string m_id;
       
        ige::scene::Event<> m_onStartHoverEvent;
        ige::scene::Event<> m_onStopHoverEvent;
        ige::scene::Event<T&> m_onDataReceivedEvent;

        bool m_isHovered = false;
    };

    template<typename T>
    DDTargetPlugin::DDTargetPlugin(const std::string& id)
        : m_id(id)
    {}

    template<typename T>
    void DDTargetPlugin::execute()
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (!m_isHovered)
                m_onStartHoverEvent.invoke();

            m_isHovered = true;

            ImGuiDragDropFlags flags = 0;
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(identifier.c_str(), target_flags))
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
