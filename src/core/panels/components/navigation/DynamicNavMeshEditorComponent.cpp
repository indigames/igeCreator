#include "core/panels/components/navigation/DynamicNavMeshEditorComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/DynamicNavMesh.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

DynamicNavMeshEditorComponent::DynamicNavMeshEditorComponent() {
    m_navMeshGroup = nullptr;
}

DynamicNavMeshEditorComponent::~DynamicNavMeshEditorComponent()
{
    if (m_navMeshGroup) {
        m_navMeshGroup->removeAllWidgets();
        m_navMeshGroup->removeAllPlugins();
    }
    m_navMeshGroup = nullptr;
}

bool DynamicNavMeshEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<DynamicNavMesh*>(comp);
}

void DynamicNavMeshEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_navMeshGroup == nullptr) {
        m_navMeshGroup = m_group->createWidget<Group>("DynamicNavMesh", false);
    }
    drawDynamicNavMesh();

    EditorComponent::redraw();
}

void DynamicNavMeshEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_navMeshGroup = m_group->createWidget<Group>("DynamicNavMesh", false);

    drawDynamicNavMesh();
}

void DynamicNavMeshEditorComponent::drawDynamicNavMesh()
{
    if (m_navMeshGroup == nullptr)
        return;
    drawNavMesh();

    auto navMesh = dynamic_cast<DynamicNavMesh*>(getComponent());
    if (navMesh == nullptr)
        return;

    std::array maxObstacles = { (int)navMesh->getMaxObstacles() };
    m_navMeshGroup->createWidget<Drag<int>>("Max Obstacles", ImGuiDataType_S32, maxObstacles, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = dynamic_cast<DynamicNavMesh*>(getComponent());
        navMesh->setMaxObstacles(val[0]);
        });

    std::array maxLayers = { (int)navMesh->getMaxLayers() };
    m_navMeshGroup->createWidget<Drag<int>>("Max Layers", ImGuiDataType_S32, maxLayers, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = dynamic_cast<DynamicNavMesh*>(getComponent());
        navMesh->setMaxLayers(val[0]);
        });
}
NS_IGE_END