#include "core/panels/components/navigation/OffMeshLinkEditorComponent.h"

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

OffMeshLinkEditorComponent::~OffMeshLinkEditorComponent()
{
    if (m_offMeshLinkGroup) {
        m_offMeshLinkGroup->removeAllWidgets();
        m_offMeshLinkGroup->removeAllPlugins();
    }
    m_offMeshLinkGroup = nullptr;
}

bool OffMeshLinkEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<OffMeshLink*>(comp);
}

void OffMeshLinkEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_offMeshLinkGroup == nullptr) {
        m_offMeshLinkGroup = m_group->createWidget<Group>("OffMeshLink", false);
    }
    drawOffMeshLink();

    EditorComponent::redraw();
}

void OffMeshLinkEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_offMeshLinkGroup = m_group->createWidget<Group>("OffMeshLink", false);

    drawOffMeshLink();
}

void OffMeshLinkEditorComponent::drawOffMeshLink()
{
    if (m_offMeshLinkGroup == nullptr)
        return;
    m_offMeshLinkGroup->removeAllWidgets();

    auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
    if (offMeshLink == nullptr)
        return;

    auto column = m_offMeshLinkGroup->createWidget<Columns<3>>();
    column->createWidget<CheckBox>("Enable", offMeshLink->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
        offMeshLink->setEnabled(val);
        });
    column->createWidget<CheckBox>("Bidirectional", offMeshLink->isBidirectional())->getOnDataChangedEvent().addListener([this](bool val) {
        auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
        offMeshLink->setBidirectional(val);
        });

    // Endpoint
    auto endPointTxt = m_offMeshLinkGroup->createWidget<TextField>("Endpoint", offMeshLink->getEndPoint() ? offMeshLink->getEndPoint()->getName() : std::string());
    endPointTxt->getOnDataChangedEvent().addListener([&](const auto& val) {
        auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
        auto obj = Editor::getCurrentScene()->findObjectByName(val);
        offMeshLink->setEndPoint(obj.get());
        });
    endPointTxt->addPlugin<DDTargetPlugin<int>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([&](auto val) {
        auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
        auto obj = Editor::getCurrentScene()->findObjectById(val);
        offMeshLink->setEndPoint(obj.get());
        redraw();
        });

    std::array radius = { offMeshLink->getRadius() };
    m_offMeshLinkGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
        offMeshLink->setRadius(val[0]);
        });

    std::array mask = { (int)offMeshLink->getMask() };
    m_offMeshLinkGroup->createWidget<Drag<int>>("Mask", ImGuiDataType_S32, mask, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
        offMeshLink->setMask(val[0]);
        });

    std::array areaId = { (int)offMeshLink->getAreaId() };
    m_offMeshLinkGroup->createWidget<Drag<int>>("Area ID", ImGuiDataType_S32, areaId, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto offMeshLink = m_targetObject->getComponent<OffMeshLink>();
        offMeshLink->setAreaId(val[0]);
        });
}
NS_IGE_END