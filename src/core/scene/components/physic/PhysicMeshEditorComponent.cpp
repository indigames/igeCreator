#include "core/scene/components/physic/PhysicMeshEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/physic/PhysicMesh.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

PhysicMeshEditorComponent::PhysicMeshEditorComponent() {
    m_physicGroup = nullptr;
}

PhysicMeshEditorComponent::~PhysicMeshEditorComponent() {
    m_physicGroup = nullptr;
}

void PhysicMeshEditorComponent::onInspectorUpdate() {
    drawPhysicMesh();
}

void PhysicMeshEditorComponent::drawPhysicMesh()
{
    // Draw common properties
    drawPhysicObject();

    // Draw mesh component
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_physicGroup->createWidget<Separator>();

    bool convex = comp->getProperty<bool>("convex", false);
    auto convexChk = m_physicGroup->createWidget<CheckBox>("ConvexHull", convex);
    convexChk->setEndOfLine(false);
    auto concaveChk = m_physicGroup->createWidget<CheckBox>("TriangleMesh", !convex);

    convexChk->getOnDataChangedEvent().addListener([this, convexChk, concaveChk](bool convex) {
        getComponent<CompoundComponent>()->setProperty("convex", convex);
        convexChk->setSelected(convex);
        concaveChk->setSelected(!convex);
    });
    concaveChk->getOnDataChangedEvent().addListener([this, convexChk, concaveChk](bool concave) {
        getComponent<CompoundComponent>()->setProperty("convex", !concave);
        convexChk->setSelected(!concave);
        concaveChk->setSelected(concave);
    });

    std::array meshIdx = { comp->getProperty<float>("meshIdx", NAN) };
    auto meshIdxWg = m_physicGroup->createWidget<Drag<float>>("MeshIndex", ImGuiDataType_S32, meshIdx, 1, 0);
    meshIdxWg->getOnDataChangedEvent().addListener([this](const auto& val) {
        getComponent<CompoundComponent>()->setProperty("meshIdx", (int)val[0]);
    });
    meshIdxWg->addPlugin<DDTargetPlugin<int>>(EDragDropID::MESH)->getOnDataReceivedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("meshIdx", val);
        setDirty();
    });

    auto txtPath = m_physicGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""));
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](const auto& txt) {
        getComponent<CompoundComponent>()->setProperty("path", txt);
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure)) {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            getComponent<CompoundComponent>()->setProperty("path", txt);
            setDirty();
        });
    }
    m_physicGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Figure (*.pyxf)", "*.pyxf" }).result();
        if (files.size() > 0) {
            getComponent<CompoundComponent>()->setProperty("path", files[0]);
            setDirty();
        }
    });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END