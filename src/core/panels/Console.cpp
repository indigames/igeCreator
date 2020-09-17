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
        Panel::_drawImpl();
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
    }
}
