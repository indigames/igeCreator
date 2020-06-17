#include <imgui.h>

#include "core/layout/Columns.h"
#include "core/widgets/Label.h"
#include "core/widgets/TextField.h"
#include "core/widgets/CheckBox.h"

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
        if (m_targetObject)
        {
            m_headerGroup->createWidget<TextField>("ID", std::to_string(m_targetObject->getId()), true);

            m_headerGroup->createWidget<TextField>("Name", m_targetObject->getName())->getOnDataChangedEvent().addListener([this](auto txt) {
                m_targetObject->setName(txt);
            });

            m_headerGroup->createWidget<CheckBox>("Active", m_targetObject->isActive())->getOnDataChangedEvent().addListener([this](bool active) {
                m_targetObject->setActive(active);
            });
        }
        
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
