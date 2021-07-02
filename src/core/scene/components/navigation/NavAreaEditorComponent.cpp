#include "core/scene/components/navigation/NavAreaEditorComponent.h"

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
    m_navAreaGroup = nullptr;
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

    auto navArea = getComponent<NavArea>();
    if (navArea == nullptr)
        return;

    std::array cost = { navArea->getAreaCost() };
    m_navAreaGroup->createWidget<Drag<float>>("Cost", ImGuiDataType_Float, cost, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navArea = getComponent<NavArea>();
        navArea->setAreaCost(val[0]);
        });
}
NS_IGE_END