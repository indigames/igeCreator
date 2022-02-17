#include "core/scene/components/navigation/NavObstacleEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavObstacle.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavObstacleEditorComponent::NavObstacleEditorComponent() {
    m_navObstacleGroup = nullptr;
}

NavObstacleEditorComponent::~NavObstacleEditorComponent() {
    m_navObstacleGroup = nullptr;
}

void NavObstacleEditorComponent::onInspectorUpdate() {   
    drawNavObstacle();
}

void NavObstacleEditorComponent::drawNavObstacle()
{
    if (m_navObstacleGroup == nullptr)
        m_navObstacleGroup = m_group->createWidget<Group>("NavObstacle", false);;
    m_navObstacleGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    std::array radius = { comp->getProperty<float>("radius", NAN) };
    auto r1 = m_navObstacleGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f);
    r1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    r1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("radius", val[0]);
    });
    
    std::array height = { comp->getProperty<float>("height", NAN) };
    auto h1 = m_navObstacleGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.f);
    h1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    h1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("height", val[0]);
    });
}
NS_IGE_END