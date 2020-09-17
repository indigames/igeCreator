#include <imgui.h>

#include "core/Panel.h"

namespace ige::creator
{
    uint64_t Panel::s_idCounter = 0;

    Panel::Panel(const std::string& name, const Panel::Settings& settings)
    {
        m_name = name;
        m_settings = settings;
        m_id = "#" + std::to_string(s_idCounter++);
    }

    Panel::~Panel()
    {
        getOnOpenedEvent().removeAllListeners();
        getOnClosedEvent().removeAllListeners();
    }

    void Panel::update(float dt)
    {
        if (!m_bInitialized)
        {
            initialize();
            m_bInitialized = true;
        }
    }

    void Panel::draw()
    {
        if(isEnable())
        {
            _drawImpl();
        }
    }

    void Panel::open()
    {
        if(!m_bIsOpened)
        {
            m_bIsOpened = true;
            m_openEvent.invoke();
        }
    }

    void Panel::close()
    {
        if(m_bIsOpened)
        {
            m_bIsOpened = false;
            m_closeEvent.invoke();
        }
    }

    void Panel::setFocus()
    {
        ImGui::SetWindowFocus((m_name).c_str());
    }

    void Panel::_drawImpl()
    {
        if (isOpened())
        {
            int windowFlags = ImGuiWindowFlags_None;

            if (m_settings.hideTitle)					windowFlags |= ImGuiWindowFlags_NoTitleBar;
            if (!m_settings.resizable)					windowFlags |= ImGuiWindowFlags_NoResize;
            if (!m_settings.movable)					windowFlags |= ImGuiWindowFlags_NoMove;
            if (!m_settings.dockable)					windowFlags |= ImGuiWindowFlags_NoDocking;
            if (m_settings.hideBackground)				windowFlags |= ImGuiWindowFlags_NoBackground;
            if (m_settings.forceHorizontalScrollbar)	windowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
            if (m_settings.forceVerticalScrollbar)		windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
            if (m_settings.allowHorizontalScrollbar)	windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
            if (!m_settings.bringToFrontOnFocus)		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (!m_settings.collapsable)				windowFlags |= ImGuiWindowFlags_NoCollapse;
            if (!m_settings.allowInputs)				windowFlags |= ImGuiWindowFlags_NoInputs;

            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();

            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;

            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;

            ImGui::SetNextWindowSizeConstraints(vMin, vMax);

            if (ImGui::Begin((m_name).c_str(), m_settings.closable ? &m_bIsOpened : nullptr, windowFlags))
            {
                m_bIsHovered = ImGui::IsWindowHovered();
                m_bIsFocused = ImGui::IsWindowFocused();

                if (!m_bIsOpened) m_closeEvent.invoke();

                auto windowPos = ImGui::GetWindowPos();
                if (m_position.x != windowPos.x || m_position.y != windowPos.y)
                {
                    m_position = windowPos;
                }
                
                auto newSize = ImGui::GetContentRegionAvail();
                if (m_size.x != newSize.x || m_size.y != newSize.y)
                {
                    getOnSizeChangedEvent().invoke(newSize);
                    m_size = newSize;
                }

                drawWidgets();
            }
            ImGui::End();
        }
    }

    const ImVec2& Panel::getPosition() const
    {
        return m_position;
    }

    const ImVec2& Panel::getSize() const
    {
        return m_size;
    }
}
