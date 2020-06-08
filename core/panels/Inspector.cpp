#include <imgui.h>

#include "core/panels/Inspector.h"

namespace ige::creator
{
    Inspector::Inspector(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
    }
    
    Inspector::~Inspector()
    {

    }

    void Inspector::initialize()
    {
        clear();
    }

    void Inspector::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void Inspector::clear()
    {

    }
}
