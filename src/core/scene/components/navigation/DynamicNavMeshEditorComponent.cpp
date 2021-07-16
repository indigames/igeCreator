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

    std::array maxObs = { comp->getProperty<float>("maxObs", NAN) };
    m_navMeshGroup->createWidget<Drag<float>>("MaxObstacle", ImGuiDataType_S32, maxObs, 1, 0)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("maxObs", (int)val[0]);
    });
    
    std::array maxLayers = { comp->getProperty<float>("maxLayers", NAN) };
    m_navMeshGroup->createWidget<Drag<float>>("MaxLayer", ImGuiDataType_S32, maxLayers, 1, 0)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("maxLayers", (int)val[0]);
    });
}
NS_IGE_END
