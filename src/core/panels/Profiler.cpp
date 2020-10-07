#include <imgui.h>

#include "core/panels/Profiler.h"


namespace ige::creator
{
    Profiler::Profiler(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
    }

    Profiler::~Profiler()
    {
        clear();
    }

    void Profiler::initialize()
    {
        clear();

        m_timeWidget = createWidget<Label>("Time");
        m_fpsWidget = createWidget<Label>("FPS");
    }

    void Profiler::_drawImpl()
    {
        // Show FPS
        m_timeWidget->setLabel("Time: " + std::to_string(static_cast<int>(1000.0f / ImGui::GetIO().Framerate)) + " ms");
        m_fpsWidget->setLabel("FPS: " + std::to_string(static_cast<int>(ImGui::GetIO().Framerate)) + " fps");
                
        Panel::_drawImpl();
    }

    void Profiler::clear()
    {
        m_timeWidget = nullptr;
        m_fpsWidget = nullptr;
        removeAllWidgets();
    }
}
