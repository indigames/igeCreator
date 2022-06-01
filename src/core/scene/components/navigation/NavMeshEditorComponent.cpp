#include "core/scene/components/navigation/NavMeshEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavMesh.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavMeshEditorComponent::NavMeshEditorComponent() {
    m_navMeshGroup = nullptr;
}

NavMeshEditorComponent::~NavMeshEditorComponent() {
    m_navMeshGroup = nullptr;
}

void NavMeshEditorComponent::onInspectorUpdate() {   
    drawNavMesh();
}

void NavMeshEditorComponent::drawNavMesh()
{
    if (m_navMeshGroup == nullptr)
        m_navMeshGroup = m_group->createWidget<Group>("NavMesh", false);;
    m_navMeshGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto column = m_navMeshGroup->createWidget<Columns<2>>();
    column->createWidget<CheckBox>("Debug", comp->getProperty<bool>("debug", false))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("debug", val);
    });
    column->createWidget<Button>("Build", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {        
        for (auto comp : getComponent<CompoundComponent>()->getComponents()) {
            auto navMesh = std::dynamic_pointer_cast<NavMesh>(comp);
            if(navMesh) navMesh->build();
        }
    });

    std::array tileSize = { comp->getProperty<float>("tileSize", NAN) };
    auto t1 = m_navMeshGroup->createWidget<Drag<float>>("TileSize", ImGuiDataType_S32, tileSize, 1, 0);
    t1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    t1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("tileSize", (int)val[0]);
    });

    std::array cellSize = { comp->getProperty<float>("cellSize", NAN) };
    auto c1 = m_navMeshGroup->createWidget<Drag<float>>("CellSize", ImGuiDataType_Float, cellSize, 0.001f, 0.f);
    c1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    c1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("cellSize", val[0]);
    });

    std::array cellHeight = { comp->getProperty<float>("cellHeight", NAN) };
    auto c2 = m_navMeshGroup->createWidget<Drag<float>>("CellHeight", ImGuiDataType_Float, cellHeight, 0.001f, 0.f);
    c2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    c2->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("cellHeight", val[0]);
    });

    std::array agentHeight = { comp->getProperty<float>("agentHeight", NAN) };
    auto a1 = m_navMeshGroup->createWidget<Drag<float>>("AgentHeight", ImGuiDataType_Float, agentHeight, 0.001f, 0.f);
    a1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    a1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("agentHeight", val[0]);
    });

    std::array agentRadius = { comp->getProperty<float>("agentRadius", NAN) };
    auto a2 = m_navMeshGroup->createWidget<Drag<float>>("AgentRadius", ImGuiDataType_Float, agentRadius, 0.001f, 0.f);
    a2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    a2->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("agentRadius", val[0]);
    });

    std::array agentMaxClimb = { comp->getProperty<float>("agentMaxClimb", NAN) };
    auto a3 = m_navMeshGroup->createWidget<Drag<float>>("AgentMaxClimb", ImGuiDataType_Float, agentMaxClimb, 0.001f, 0.f);
    a3->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    a3->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("agentMaxClimb", val[0]);
    });

    std::array agentMaxSlope = { comp->getProperty<float>("agentMaxSlope", NAN) };
    auto a4 = m_navMeshGroup->createWidget<Drag<float>>("AgentMaxSlope", ImGuiDataType_Float, agentMaxSlope, 0.001f, 0.f);
    a4->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    a4->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("agentMaxSlope", val[0]);
    });

    std::array regionMinSize = { comp->getProperty<float>("regionMinSize", NAN) };
    auto r1 = m_navMeshGroup->createWidget<Drag<float>>("RegionMinSize", ImGuiDataType_Float, regionMinSize, 0.001f, 0.f);
    r1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    r1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("regionMinSize", val[0]);
    });

    std::array regionMergeSize = { comp->getProperty<float>("regionMergeSize", NAN) };
    auto r2 = m_navMeshGroup->createWidget<Drag<float>>("RegionMergeSize", ImGuiDataType_Float, regionMergeSize, 0.001f, 0.f);
    r2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    r2->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("regionMergeSize", val[0]);
    });

    std::array edgeMaxLength = { comp->getProperty<float>("edgeMaxLength", NAN) };
    auto e1 = m_navMeshGroup->createWidget<Drag<float>>("EdgeMaxLength", ImGuiDataType_Float, edgeMaxLength, 0.001f, 0.f);
    e1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    e1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("edgeMaxLength", val[0]);
    });

    std::array edgeMaxError = { comp->getProperty<float>("edgeMaxError", NAN) };
    auto e2 = m_navMeshGroup->createWidget<Drag<float>>("EdgeMaxError", ImGuiDataType_Float, edgeMaxError, 0.001f, 0.f);
    e2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    e2->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("edgeMaxError", val[0]);
    });

    std::array detailSampleDistance = { comp->getProperty<float>("sampleDist", NAN) };
    auto s1 = m_navMeshGroup->createWidget<Drag<float>>("SampleDistance", ImGuiDataType_Float, detailSampleDistance, 0.001f, 0.f);
    s1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    s1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("sampleDist", val[0]);
    });

    std::array detailSampleMaxError = { comp->getProperty<float>("sampleError", NAN) };
    auto s2 = m_navMeshGroup->createWidget<Drag<float>>("SampleMaxError", ImGuiDataType_Float, detailSampleMaxError, 0.001f, 0.f);
    s2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    s2->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("sampleError", val[0]);
    });

    auto aabbPading = comp->getProperty<Vec3>("padding", { NAN, NAN, NAN });
    std::array padding = { aabbPading.X(), aabbPading.Y(), aabbPading.Z() };
    auto p1 = m_navMeshGroup->createWidget<Drag<float, 3>>("Padding", ImGuiDataType_Float, padding, 0.001f, 0.f);
    p1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    p1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("padding", { val[0], val[1], val[2] });
    });

    auto partitionType = comp->getProperty<int>("partType", 0);
    auto partitionCombo = m_navMeshGroup->createWidget<ComboBox>("PartitionType", partitionType);
    partitionCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("partType", val);
            setDirty();
        }
    });
    partitionCombo->setEndOfLine(false);
    partitionCombo->addChoice(0, "Watershed");
    partitionCombo->addChoice(1, "Monotone");
    partitionCombo->setEndOfLine(true);
}
NS_IGE_END