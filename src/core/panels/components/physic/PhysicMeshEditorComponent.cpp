#include "core/panels/components/physic/PhysicMeshEditorComponent.h"

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

PhysicMeshEditorComponent::~PhysicMeshEditorComponent()
{
    if (m_physicGroup) {
        m_physicGroup->removeAllWidgets();
        m_physicGroup->removeAllPlugins();
    }
    m_physicGroup = nullptr;
}

bool PhysicMeshEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<PhysicMesh*>(comp);
}

void PhysicMeshEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_physicGroup == nullptr) {
        m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);
    }
    drawPhysicMesh();

    EditorComponent::redraw();
}

void PhysicMeshEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_physicGroup = m_group->createWidget<Group>("PhysicGroup", false);

    drawPhysicMesh();
}

void PhysicMeshEditorComponent::drawPhysicMesh()
{
    drawPhysicObject();

    auto physicComp = m_targetObject->getComponent<PhysicMesh>();
    if (physicComp == nullptr)
        return;

    m_physicGroup->createWidget<Separator>();

    bool convex = physicComp->isConvex();
    auto convexChk = m_physicGroup->createWidget<CheckBox>("Convex Hull", convex);
    convexChk->setEndOfLine(false);

    auto concaveChk = m_physicGroup->createWidget<CheckBox>("Triangle Mesh", !convex);
    convexChk->getOnDataChangedEvent().addListener([this, convexChk, concaveChk](bool convex) {
        auto physicComp = m_targetObject->getComponent<PhysicMesh>();
        physicComp->setConvex(convex);
        convexChk->setSelected(physicComp->isConvex());
        concaveChk->setSelected(!physicComp->isConvex());
        });

    concaveChk->getOnDataChangedEvent().addListener([this, convexChk, concaveChk](bool concave) {
        auto physicComp = m_targetObject->getComponent<PhysicMesh>();
        physicComp->setConvex(!concave);
        convexChk->setSelected(physicComp->isConvex());
        concaveChk->setSelected(!physicComp->isConvex());
        });

    std::array meshIdx = { physicComp->getMeshIndex() };
    auto meshIdxWg = m_physicGroup->createWidget<Drag<int>>("Mesh Index", ImGuiDataType_S32, meshIdx, 1, 0);
    meshIdxWg->getOnDataChangedEvent().addListener([this](auto& val) {
        auto physicComp = m_targetObject->getComponent<PhysicMesh>();
        physicComp->setMeshIndex(val[0]);
        });
    meshIdxWg->addPlugin<DDTargetPlugin<int>>(EDragDropID::MESH)->getOnDataReceivedEvent().addListener([this](auto val) {
        auto physicComp = m_targetObject->getComponent<PhysicMesh>();
        physicComp->setMeshIndex(val);
        dirty();
        });

    auto txtPath = m_physicGroup->createWidget<TextField>("Path", physicComp->getPath());
    //! Mark Not End OF line for browse btn
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto physicComp = m_targetObject->getComponent<PhysicMesh>();
        physicComp->setPath(txt);
        });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto physicComp = m_targetObject->getComponent<PhysicMesh>();
            physicComp->setPath(txt);
            dirty();
            });
    }
    m_physicGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Figure (*.pyxf)", "*.pyxf" }).result();
        if (files.size() > 0)
        {
            auto physicComp = m_targetObject->getComponent<PhysicMesh>();
            physicComp->setPath(files[0]);
            dirty();
        }
        });

    // Draw constraints
    drawPhysicConstraints();
}
NS_IGE_END