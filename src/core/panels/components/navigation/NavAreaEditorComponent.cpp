#include "core/panels/components/navigation/NavAreaEditorComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavArea.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavAreaEditorComponent::NavAreaEditorComponent() {
    m_navAreaGroup = nullptr;
}

NavAreaEditorComponent::~NavAreaEditorComponent()
{
    if (m_navAreaGroup) {
        m_navAreaGroup->removeAllWidgets();
        m_navAreaGroup->removeAllPlugins();
    }
    m_navAreaGroup = nullptr;
}

bool NavAreaEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<NavArea*>(comp);
}

void NavAreaEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_navAreaGroup == nullptr) {
        m_navAreaGroup = m_group->createWidget<Group>("NavArea", false);
    }
    drawNavArea();

    EditorComponent::redraw();
}

void NavAreaEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_navAreaGroup = m_group->createWidget<Group>("NavArea", false);

    drawNavArea();
}

void NavAreaEditorComponent::drawNavArea()
{
    if (m_navAreaGroup == nullptr)
        return;
    m_navAreaGroup->removeAllWidgets();

    auto navArea = dynamic_cast<NavArea*>(getComponent());
    if (navArea == nullptr)
        return;

    std::array cost = { navArea->getAreaCost() };
    m_navAreaGroup->createWidget<Drag<float>>("Cost", ImGuiDataType_Float, cost, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navArea = dynamic_cast<NavArea*>(getComponent());
        navArea->setAreaCost(val[0]);
        });
}
NS_IGE_END