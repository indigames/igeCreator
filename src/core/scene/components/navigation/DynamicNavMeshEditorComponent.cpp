#include "core/scene/components/navigation/DynamicNavMeshEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/DynamicNavMesh.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

DynamicNavMeshEditorComponent::DynamicNavMeshEditorComponent() {
    m_navMeshGroup = nullptr;
}

DynamicNavMeshEditorComponent::~DynamicNavMeshEditorComponent() {
    m_navMeshGroup = nullptr;
}

void DynamicNavMeshEditorComponent::onInspectorUpdate() {
    drawDynamicNavMesh();
}

void DynamicNavMeshEditorComponent::drawDynamicNavMesh() {
    // Draw base NavMesh
    drawNavMesh();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_navMeshGroup->createWidget<Separator>();

    std::array maxObs = { comp->getProperty<int>("maxObs", 1024) };
    m_navMeshGroup->createWidget<Drag<int>>("MaxObstacle", ImGuiDataType_S32, maxObs, 1, 0)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("maxObs", val[0]);
    });
    
    std::array maxLayers = { comp->getProperty<int>("maxLayers", 16) };
    m_navMeshGroup->createWidget<Drag<int>>("MaxLayer", ImGuiDataType_S32, maxLayers, 1, 0)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("maxLayers", val[0]);
    });
}
NS_IGE_END
