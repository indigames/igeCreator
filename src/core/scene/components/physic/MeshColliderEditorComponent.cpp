#include "core/scene/components/physic/MeshColliderEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "core/Editor.h"

#include <core/layout/Group.h>

#include "components/physic/collider/MeshCollider.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

MeshColliderEditorComponent::MeshColliderEditorComponent() {
    m_physicGroup = nullptr;
}

MeshColliderEditorComponent::~MeshColliderEditorComponent() {
    m_physicGroup = nullptr;
}

void MeshColliderEditorComponent::onInspectorUpdate() {
    drawMeshCollider();
}

void MeshColliderEditorComponent::drawMeshCollider()
{
    if (m_physicGroup == nullptr)
        m_physicGroup = m_group->createWidget<Group>("ColliderGroup", false);;
    m_physicGroup->removeAllWidgets();

    // Draw mesh component
    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_physicGroup->createWidget<Separator>();

    bool convex = comp->getProperty<bool>("convex", false);
    auto convexChk = m_physicGroup->createWidget<CheckBox>("ConvexHull", convex);
    convexChk->setEndOfLine(false);
    auto concaveChk = m_physicGroup->createWidget<CheckBox>("TriangleMesh", !convex);
    convexChk->setSelected(convex);
    concaveChk->setSelected(!convex);

    convexChk->getOnDataChangedEvent().addListener([this, convexChk, concaveChk](bool convex) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("convex", convex);
        setDirty();
        getComponent<CompoundComponent>()->setDirty();
    });
    concaveChk->getOnDataChangedEvent().addListener([this, convexChk, concaveChk](bool concave) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("convex", !concave);
        setDirty();
        getComponent<CompoundComponent>()->setDirty();
    });

    std::array meshIdx = { comp->getProperty<float>("meshIdx", NAN) };
    auto meshIdxWg = m_physicGroup->createWidget<Drag<float>>("MeshIndex", ImGuiDataType_S32, meshIdx, 1, -1, comp->getProperty<float>("maxIdx", 0) - 1);
    meshIdxWg->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    meshIdxWg->getOnDataChangedEvent().addListener([this](const auto& val) {
        getComponent<CompoundComponent>()->setProperty("meshIdx", (int)val[0]);
    });
    meshIdxWg->addPlugin<DDTargetPlugin<int>>(EDragDropID::MESH)->getOnDataReceivedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("meshIdx", val);
        setDirty();
    });

    std::array margin = { comp->getProperty<float>("margin", NAN) };
    auto m2 = m_physicGroup->createWidget<Drag<float>>("Margin", ImGuiDataType_Float, margin, 0.001f, 0.0f);
    m2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    m2->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("margin", val[0]);
    });
}
NS_IGE_END