#pragma once

#include <string>

#include "core/IDrawable.h"
#include "core/Container.h"

#include <event/Event.h>
#include <imgui.h>

namespace ige::creator
{
    class Panel: public IDrawable, public Container
    {
    public:
        struct Settings
        {
            bool closable					= false;
            bool resizable					= true;
            bool movable					= true;
            bool dockable					= false;
            bool hideBackground				= false;
            bool forceHorizontalScrollbar	= false;
            bool forceVerticalScrollbar		= false;
            bool allowHorizontalScrollbar	= false;
            bool bringToFrontOnFocus		= true;
            bool collapsable				= false;
            bool allowInputs				= true;
            bool autoSize					= false;
            bool hideTitle					= false;
        };
        
    public:
        Panel(const std::string& name = "", Panel::Settings setting = {});
        virtual ~Panel();

        void draw() override;
        virtual void update(float dt);
        
        void open();
        void close();
        bool isOpened() const { return m_bIsOpened; }

        void setFocus();
        bool isFocused() const { return m_bIsFocused; }
        bool isHovered() const { return m_bIsHovered; }

        std::string getName() const { return m_name; }
        void setName(const std::string& name) { m_name = name; }

        std::string getId() const { return m_id; }
        
        void setEnable(bool enable) { m_bEnabled = enable; }
        bool isEnable() const { return m_bEnabled; }

        const ImVec2& getSize() const;
        const ImVec2& getPosition() const;
        const ImVec2& getScrollPosition() const;

        ige::scene::Event<>& getOnOpenedEvent() { return m_openEvent; }
        ige::scene::Event<>& getOnClosedEvent() { return m_closeEvent; }
        ige::scene::Event<ImVec2&>& getOnSizeChangedEvent() { return m_sizeChangedEvent; }
        ige::scene::Event<ImVec2&>& getOnPositionChangedEvent() { return m_positionChangedEvent; }

    protected:
        virtual void initialize() {};
        virtual void _drawImpl();

        bool m_bInitialized = false;
        bool m_bEnabled = true;
        bool m_bIsOpened = true;
        bool m_bIsFocused = false;
        bool m_bIsHovered = false;
        std::string m_id;

        ige::scene::Event<> m_openEvent;
        ige::scene::Event<> m_closeEvent;
        ige::scene::Event<ImVec2&> m_sizeChangedEvent;
        ige::scene::Event<ImVec2&> m_positionChangedEvent;

    protected:
        std::string m_name;
        Settings m_settings;
        static uint64_t s_idCounter;

        ImVec2 m_position = {0.f, 0.f};
        ImVec2 m_scrollPosition = { 0.f, 0.f };

        ImVec2 m_size = {0.f, 0.f};
        bool m_bSizeChanged = false;
    };
}