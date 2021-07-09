#include "core/scene/components/navigation/NavAreaEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavArea.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavAreaEditorComponent::NavAreaEditorComponent() {
    m_navAreaGroup = nullptr;
}

NavAreaEditorComponent::~NavAreaEditorComponent() {
    m_navAreaGroup = nullptr;
}

void NavAreaEditorComponent::onInspectorUpdate() {
   drawNavArea();
}

void NavAreaEditorComponent::drawNavArea()
{
    if (m_navAreaGroup == nullptr)
        m_navAreaGroup = m_group->createWidget<Group>("NavArea", false);;
    m_navAreaGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    std::array id = { comp->getProperty<int>("id", 0) };
    m_navAreaGroup->createWidget<Drag<int>>("AreaID", ImGuiDataType_S32, id, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("id", val[0]);
    });

    std::array cost = { comp->getProperty<int>("cost", 0) };
    m_navAreaGroup->createWidget<Drag<int>>("AreaCost", ImGuiDataType_S32, cost, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("cost", val[0]);
    });
}
NS_IGE_END