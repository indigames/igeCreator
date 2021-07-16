#include "core/scene/components/navigation/OffMeshLinkEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/OffMeshLink.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

#include "core/Editor.h"
#include <scene/Scene.h>

NS_IGE_BEGIN

OffMeshLinkEditorComponent::OffMeshLinkEditorComponent() {
    m_offMeshLinkGroup = nullptr;
}

OffMeshLinkEditorComponent::~OffMeshLinkEditorComponent() {
    m_offMeshLinkGroup = nullptr;
}

void OffMeshLinkEditorComponent::onInspectorUpdate() {
    drawOffMeshLink();
}

void OffMeshLinkEditorComponent::drawOffMeshLink()
{
    if (m_offMeshLinkGroup == nullptr)
        m_offMeshLinkGroup = m_group->createWidget<Group>("OffMeshLink", false);;
    m_offMeshLinkGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_offMeshLinkGroup->createWidget<CheckBox>("Bidirectional", comp->getProperty<bool>("2way", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("2way", val);
    });

    // Endpoint
    auto endPointObj = Editor::getCurrentScene()->findObjectByUUID(comp->getProperty<std::string>("endUuid", ""));
    auto endPointTxt = m_offMeshLinkGroup->createWidget<TextField>("Endpoint", endPointObj ? endPointObj->getName() : "");
    endPointTxt->getOnDataChangedEvent().addListener([&](const auto& val) {
        getComponent<CompoundComponent>()->setProperty("endUuid", val);
        setDirty();
    });
    endPointTxt->addPlugin<DDTargetPlugin<int>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([&](auto val) {
        getComponent<CompoundComponent>()->setProperty("endUuid", val);
        setDirty();
    });

    std::array radius = { comp->getProperty<float>("radius", NAN) };
    m_offMeshLinkGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("radius", val[0]);
    });

    std::array mask = { comp->getProperty<float>("mask", NAN) };
    m_offMeshLinkGroup->createWidget<Drag<float>>("Mask", ImGuiDataType_S32, mask, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("mask", (int)val[0]);
    });

    std::array areaId = { comp->getProperty<float>("areaId", NAN) };
    m_offMeshLinkGroup->createWidget<Drag<float>>("AreaID", ImGuiDataType_S32, areaId, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("areaId", (int)val[0]);
    });
}
NS_IGE_END