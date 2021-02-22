#include <imgui.h>
#include <ctime>

#include "core/panels/Console.h"
#include "core/Canvas.h"
#include "core/Editor.h"
#include "core/widgets/Label.h"

#include <utils/GraphicsHelper.h>
using namespace ige::scene;

#ifdef __cplusplus
extern "C" {
#endif
    extern LoggerFunc gLoggerFunc;
    void onRawLogged(const char* message)
    {
        if (ige::creator::Editor::getInstance()
            && ige::creator::Editor::getInstance()->getCanvas()
            && ige::creator::Editor::getInstance()->getCanvas()->getConsole())
        {
            ige::creator::Editor::getInstance()->getCanvas()->getConsole()->onLogged(message);
        }
    }  
#ifdef __cplusplus
}
#endif


namespace ige::creator
{
    Console::Console(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        gLoggerFunc = &onRawLogged;
    }
    
    Console::~Console()
    {

    }

    void Console::initialize()
    {
        clear();
    }

    void Console::_drawImpl()
    {
        //Panel::_drawImpl();
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
                    getOnPositionChangedEvent().invoke(windowPos);
                    m_position = windowPos;
                }

                auto newSize = ImGui::GetContentRegionAvail();
                if (m_size.x != newSize.x || m_size.y != newSize.y)
                {
                    getOnSizeChangedEvent().invoke(newSize);
                    m_size = newSize;
                }

                drawWidgets();

                if (_scrollToBottom == 1)
                    _scrollToBottom++;
                else if (_scrollToBottom > 1) {
                    auto pos = ImGui::GetScrollMaxY();
                    ImGui::SetScrollY(pos);
                    _scrollToBottom = 0;
                }
            }
            ImGui::End();
        }
    }

    void Console::clear()
    {
        if (m_logGroup)
        {
            m_logGroup->removeAllWidgets();
            m_logGroup->removeAllPlugins();
        }
        m_logGroup = nullptr;
        removeAllWidgets();
    }

    void Console::onLogged(const char* message)
    {
        if(m_logGroup == nullptr)
            m_logGroup = createWidget<Group>("Console Log", false, false);
        
        time_t rawtime;
        time(&rawtime);
        auto timeinfo = localtime(&rawtime);

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
        auto msg = std::string(buffer) + "\t" + std::string(message);

        m_logGroup->createWidget<Label>(msg);
        _scrollToBottom = 1;
    }
}
