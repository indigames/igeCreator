#include <imgui.h>

#include "core/layout/Columns.h"
#include "core/widgets/Label.h"
#include "core/widgets/TextField.h"

#include "core/panels/Inspector.h"
#include "core/Editor.h"

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

        m_headerGroup = createWidget<Group>("Inspector_Header", false);
        auto columns = m_headerGroup->createWidget<Columns<2>>();

        std::string input = m_targetObject ? m_targetObject->getName() : "sasdasdsad";
        columns->createWidget<TextField>("txtName", input);

        m_componentGroup = createWidget<Group>("Inspector_Components", false);

    }

    void Inspector::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void Inspector::clear()
    {
        removeAllWidgets();
    }

    void Inspector::setTargetObject(std::shared_ptr<SceneObject> obj)
    { 
        m_targetObject = obj;

        initialize();
    }
}
