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

    std::array id = { comp->getProperty<float>("id", NAN) };
    m_navAreaGroup->createWidget<Drag<float>>("AreaID", ImGuiDataType_S32, id, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("id", (int)val[0]);
    });

    std::array cost = { comp->getProperty<float>("cost", NAN) };
    m_navAreaGroup->createWidget<Drag<float>>("AreaCost", ImGuiDataType_S32, cost, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("cost", (int)val[0]);
    });
}
NS_IGE_END