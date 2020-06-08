#include <imgui.h>

#include "core/panels/Hierarchy.h"

namespace ige::creator
{
    Hierarchy::Hierarchy(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
    }
    
    Hierarchy::~Hierarchy()
    {

    }

    void Hierarchy::initialize()
    {
        clear();
    }

    void Hierarchy::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void Hierarchy::clear()
    {

    }
}
