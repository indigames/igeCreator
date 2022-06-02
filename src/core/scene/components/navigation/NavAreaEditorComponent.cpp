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
    auto a1 = m_navAreaGroup->createWidget<Drag<float>>("AreaID", ImGuiDataType_S32, id, 1, 0);
    a1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    a1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("id", (int)val[0]);
    });
}
NS_IGE_END