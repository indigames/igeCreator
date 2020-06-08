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

    Panel::~Panel() {}

    void Panel::draw()
    {
        if (!m_bInitialized)
        {
            initialize();
            m_bInitialized = true;
        }

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

            ImGui::SetNextWindowSizeConstraints({ 0.0f, 0.0f }, { 10000.f, 10000.f });

            if (ImGui::Begin((m_name).c_str(), m_settings.closable ? &m_bIsOpened : nullptr, windowFlags))
            {
                m_bIsHovered = ImGui::IsWindowHovered();
                m_bIsFocused = ImGui::IsWindowFocused();

                if (!m_bIsOpened) m_closeEvent.invoke();

                if(!m_settings.autoSize)
                    updateSize();
                
                updatePosition();

                drawWidgets();
            }
            ImGui::End();
        }
    }

    void Panel::setPosition(const ImVec2& pos) {
        m_position = pos;
        m_bPositionChanged = true;
    }

    const ImVec2& Panel::getPosition() const
    {
        return m_position;
    }

    void Panel::setSize(const ImVec2& size)
    {
        m_size = size;
        m_bSizeChanged = true;
    }

    const ImVec2& Panel::getSize() const
    {
        return m_size;
    }

    void Panel::setAlign(Panel::E_HAlign hAlign, Panel::E_VAlign vAlign)
    {
        m_hAlign = hAlign;
        m_vAlign = vAlign;
        m_bAlignChanged = true;
    }

    void Panel::updatePosition()
    {
        if(m_bPositionChanged || m_bAlignChanged)
        {
            ImVec2 offset = {0.f, 0.f};

            switch (m_hAlign)
            {
            case Panel::E_HAlign::CENTER:
                offset.x -= m_size.x / 2.0f;
                break;
            case Panel::E_HAlign::RIGHT:
                offset.x -= m_size.x;
                break;
            default:
                break;
            }

            switch (m_vAlign)
            {
            case Panel::E_VAlign::MIDDLE:
                offset.y -= m_size.y / 2.0f;
                break;
            case Panel::E_VAlign::BOTTOM:
                offset.y -= m_size.y;
                break;
            default:
                break;
            }
            
            ImGui::SetWindowPos(ImVec2(m_position.x + offset.x, m_position.y + offset.y), ImGuiCond_Always);

            m_bPositionChanged = false;
            m_bAlignChanged = false;
        }
    }

    void Panel::updateSize()
    {
        if (m_bSizeChanged)
        {
            ImGui::SetWindowSize(m_size, ImGuiCond_Always);
            m_bSizeChanged = false;
        }
    }
}
