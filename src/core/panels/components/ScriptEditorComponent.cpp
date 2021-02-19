#include "core/panels/components/ScriptEditorComponent.h"

#include <core/layout/Group.h>

#include "components/ScriptComponent.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

ScriptEditorComponent::ScriptEditorComponent() {
    m_scriptCompGroup = nullptr;
}

ScriptEditorComponent::~ScriptEditorComponent()
{
    if (m_scriptCompGroup) {
        m_scriptCompGroup->removeAllWidgets();
        m_scriptCompGroup->removeAllPlugins();
    }
    m_scriptCompGroup = nullptr;
}

bool ScriptEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<ScriptComponent*>(comp);
}

void ScriptEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_scriptCompGroup == nullptr) {
        m_scriptCompGroup = m_group->createWidget<Group>("ScriptGroup", false);
    }
    drawScriptComponent();

    EditorComponent::redraw();
}

void ScriptEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_scriptCompGroup = m_group->createWidget<Group>("ScriptGroup", false);

    drawScriptComponent();
}

void ScriptEditorComponent::drawScriptComponent()
{
    if (m_scriptCompGroup == nullptr)
        return;
    m_scriptCompGroup->removeAllWidgets();

    auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
    if (scriptComp == nullptr)
        return;
    
    auto txtPath = m_scriptCompGroup->createWidget<TextField>("Path", scriptComp->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
        scriptComp->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Script))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
            scriptComp->setPath(txt);
            m_bisDirty = true;
            });
    }

    m_scriptCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Script (*.py)", "*.py" }).result();
        if (files.size() > 0)
        {
            auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
            scriptComp->setPath(files[0]);
            m_bisDirty = true;
        }
        });
}
NS_IGE_END