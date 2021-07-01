#include <imgui.h>
#include <ctime>

#include "core/panels/Console.h"
#include "core/Canvas.h"
#include "core/Editor.h"
#include "core/widgets/Label.h"
#include "core/widgets/Button.h"
#include "core/widgets/Drag.h"
#include "core/task/TaskManager.h"
#include "core/menu/ContextMenu.h"
#include "core/menu/MenuItem.h"
#include "core/widgets/Button.h"
#include "core/widgets/CheckBox.h"

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
        clear();
    }

    void Console::initialize()
    {
        clear();

        m_topGroup = createWidget<Group>("Console Top", false, false);

        auto columns = m_topGroup->createWidget<Columns<10>>(128.f);

        columns->createWidget<Button>("Clear", ImVec2(64.f, 20.f), true, false)->getOnClickEvent().addListener([this](auto widget) {
            TaskManager::getInstance()->addTask([this]() {
                initialize();
            });
        });

        auto clearChk = columns->createWidget<CheckBox>("AutoClear", m_bAutoClearOnStart);
        clearChk->setEndOfLine(false);
        clearChk->getOnDataChangedEvent().addListener([this](bool val) {
            m_bAutoClearOnStart = val;
        });

        auto scrollChk = columns->createWidget<CheckBox>("AutoScroll", m_bAutoScroll);
        scrollChk->setEndOfLine(false);
        scrollChk->getOnDataChangedEvent().addListener([this](bool val) {
            m_bAutoScroll = val;
        });

        std::array maxLines = { m_maxLines };
        columns->createWidget<Drag<int>>("Rows", ImGuiDataType_S32, maxLines, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            m_maxLines = val[0];
        });

        m_logGroup = createWidget<Group>("Console Log", false, false);
        auto ctxMenu = m_logGroup->addPlugin<WindowContextMenu>("Console_Context");
        ctxMenu->createWidget<MenuItem>("Clear")->getOnClickEvent().addListener([this](auto widget) {
            TaskManager::getInstance()->addTask([this]() {
                initialize();
            });
        });
    }

    void Console::_drawImpl()
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
                    getOnPositionChangedEvent().invoke(windowPos);
                    m_position = windowPos;
                }

                auto newSize = ImGui::GetContentRegionAvail();
                if (m_size.x != newSize.x || m_size.y != newSize.y)
                {
                    getOnSizeChangedEvent().invoke(newSize);
                    m_size = newSize;
                }

                // draw top group
                m_topGroup->draw();
                
                // draw scrollable group
                ImGui::BeginChild("log_group_child");
                    m_logGroup->draw();
                    if (m_bAutoScroll && _scrollToBottom > 1) {
                        auto pos = ImGui::GetScrollMaxY();
                        ImGui::SetScrollY(pos);
                        _scrollToBottom = 0;
                    }
                ImGui::EndChild();
            }
            ImGui::End();
        }
    }

    void Console::clear()
    {
        if (m_logGroup)
        {
            m_logGroup->removeAllWidgets();
        }
        m_logGroup = nullptr;
        removeAllWidgets();
    }

    void Console::onLogged(const char* message)
    {
        if (m_logGroup == nullptr)
            initialize();

        time_t rawtime;
        time(&rawtime);
        auto timeinfo = localtime(&rawtime);

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
        auto msg = std::string(buffer) + "\t" + std::string(message);

        while (m_logIds.size() > m_maxLines)
        {
            auto id = m_logIds.front();
            m_logIds.pop_front();
            m_logGroup->removeWidgetById(id);
        }

        m_logIds.push_back(m_logGroup->createWidget<Label>(msg)->getId());

        _scrollToBottom++;
    }

    void Console::clearAllLogs()
    {
        initialize();
    }
}
