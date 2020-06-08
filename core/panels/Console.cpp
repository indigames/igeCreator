#include <imgui.h>

#include "core/panels/Console.h"

namespace ige::creator
{
    Console::Console(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
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

    }
}
