#include "core/panels/components/navigation/NavMeshEditorComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavMesh.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavMeshEditorComponent::NavMeshEditorComponent() {
    m_navMeshGroup = nullptr;
}

NavMeshEditorComponent::~NavMeshEditorComponent()
{
    m_navMeshGroup = nullptr;
}

void NavMeshEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_navMeshGroup == nullptr) {
        m_navMeshGroup = m_group->createWidget<Group>("NavMesh", false);
    }
    drawNavMesh();

    EditorComponent::redraw();
}

void NavMeshEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_navMeshGroup = m_group->createWidget<Group>("NavMesh", false);

    drawNavMesh();
}

void NavMeshEditorComponent::drawNavMesh()
{
    if (m_navMeshGroup == nullptr)
        return;
    m_navMeshGroup->removeAllWidgets();

    auto navMesh = getComponent<NavMesh>();
    if (navMesh == nullptr)
        return;

    auto column = m_navMeshGroup->createWidget<Columns<3>>();
    column->createWidget<CheckBox>("Enable", navMesh->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setEnabled(val);
        });
    column->createWidget<CheckBox>("Debug", navMesh->isShowDebug())->getOnDataChangedEvent().addListener([this](bool val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setShowDebug(val);
        });
    column->createWidget<Button>("Build", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->build();
        });

    std::array tileSize = { navMesh->getTileSize() };
    m_navMeshGroup->createWidget<Drag<int>>("Tile Size", ImGuiDataType_S32, tileSize, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setTileSize(val[0]);
        });

    std::array cellSize = { navMesh->getCellSize() };
    m_navMeshGroup->createWidget<Drag<float>>("Cell Size", ImGuiDataType_Float, cellSize, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setCellSize(val[0]);
        });

    std::array cellHeight = { navMesh->getCellHeight() };
    m_navMeshGroup->createWidget<Drag<float>>("Cell Height", ImGuiDataType_Float, cellHeight, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setCellHeight(val[0]);
        });

    std::array agentRadius = { navMesh->getAgentRadius() };
    m_navMeshGroup->createWidget<Drag<float>>("Agent Radius", ImGuiDataType_Float, agentRadius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setAgentRadius(val[0]);
        });

    std::array agentHeight = { navMesh->getAgentHeight() };
    m_navMeshGroup->createWidget<Drag<float>>("Agent Height", ImGuiDataType_Float, agentHeight, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setAgentHeight(val[0]);
        });

    std::array agentMaxClimb = { navMesh->getAgentMaxClimb() };
    m_navMeshGroup->createWidget<Drag<float>>("Agent Max Climb", ImGuiDataType_Float, agentMaxClimb, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setAgentMaxClimb(val[0]);
        });

    std::array agentMaxSlope = { navMesh->getAgentMaxSlope() };
    m_navMeshGroup->createWidget<Drag<float>>("Agent Max Slope", ImGuiDataType_Float, agentMaxSlope, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setAgentMaxSlope(val[0]);
        });

    std::array regionMinSize = { navMesh->getRegionMinSize() };
    m_navMeshGroup->createWidget<Drag<float>>("Region Min Size", ImGuiDataType_Float, regionMinSize, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setRegionMinSize(val[0]);
        });

    std::array regionMergeSize = { navMesh->getRegionMergeSize() };
    m_navMeshGroup->createWidget<Drag<float>>("Region Merge Size", ImGuiDataType_Float, regionMergeSize, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setRegionMergeSize(val[0]);
        });

    std::array edgeMaxLength = { navMesh->getEdgeMaxLength() };
    m_navMeshGroup->createWidget<Drag<float>>("Edge Max Length", ImGuiDataType_Float, edgeMaxLength, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setEdgeMaxLength(val[0]);
        });

    std::array edgeMaxError = { navMesh->getEdgeMaxError() };
    m_navMeshGroup->createWidget<Drag<float>>("Edge Max Error", ImGuiDataType_Float, edgeMaxError, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setEdgeMaxError(val[0]);
        });

    std::array detailSampleDistance = { navMesh->getDetailSampleDistance() };
    m_navMeshGroup->createWidget<Drag<float>>("Detail Sample Distance", ImGuiDataType_Float, detailSampleDistance, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setDetailSampleDistance(val[0]);
        });

    std::array detailSampleMaxError = { navMesh->getDetailSampleMaxError() };
    m_navMeshGroup->createWidget<Drag<float>>("Detail Sample Max Error", ImGuiDataType_Float, detailSampleMaxError, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setDetailSampleMaxError(val[0]);
        });

    std::array padding = { navMesh->getPadding().X(), navMesh->getPadding().Y(), navMesh->getPadding().Z() };
    m_navMeshGroup->createWidget<Drag<float, 3>>("AABB Padding", ImGuiDataType_Float, padding, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setPadding({ val[0], val[1], val[2] });
        });

    std::array partitionType = { (int)navMesh->getPartitionType() };
    m_navMeshGroup->createWidget<Drag<int>>("Partition Type", ImGuiDataType_S32, partitionType, 1, (int)(NavMesh::EPartitionType::WATERSHED), (int)(NavMesh::EPartitionType::MONOTONE))->getOnDataChangedEvent().addListener([this](auto val) {
        auto navMesh = getComponent<NavMesh>();
        navMesh->setPartitionType((NavMesh::EPartitionType)val[0]);
        });
}
NS_IGE_END