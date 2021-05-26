#include "core/panels/components/ScriptEditorComponent.h"

#include "core/layout/Group.h"
#include "core/widgets/Widgets.h"
#include "core/FileHandle.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Hierarchy.h"
#include "core/menu/ContextMenu.h"
#include "core/menu/MenuItem.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"
#include "core/filesystem/FileSystemWatcher.h"

#include <components/ScriptComponent.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

NS_IGE_BEGIN

ScriptEditorComponent::ScriptEditorComponent() {
    m_scriptCompGroup = nullptr;
}

ScriptEditorComponent::~ScriptEditorComponent()
{
    if (m_watchId != 0)
    {
        fs::watcher::unwatch(m_watchId);
        m_watchId = 0;
    }

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
            dirty();
        });
    }

    m_scriptCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Script (*.py)", "*.py" }).result();
        if (files.size() > 0)
        {
            auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
            scriptComp->setPath(files[0]);
            dirty();
        }
    });

    if (m_watchId != 0)
    {
        fs::watcher::unwatch(m_watchId);
        m_watchId = 0;
    }

    if (scriptComp->getPath().length() > 0)
    {
        m_watchId = fs::watcher::watch(fs::path(scriptComp->getPath()), false, false, std::chrono::milliseconds(1000), [this](const auto&, bool) {
            auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
            scriptComp->setPath(scriptComp->getPath(), true);
            dirty();
         });
    }

    for (const auto& pair : scriptComp->getMembers())
    {
        auto key = pair.first;
        auto value = pair.second;
        
        switch (value.getType()) 
        {
            case Value::Type::INTEGER:
            case Value::Type::UNSIGNED:
            {
                std::array val = { value.asInt() };
                m_scriptCompGroup->createWidget<Drag<int>>(key, ImGuiDataType_S32, val)->getOnDataChangedEvent().addListener([key, this](auto& val) {                    
                    auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                    scriptComp->onMemberValueChanged(key, Value(val[0]));
                });
            }
            break;

            case Value::Type::DOUBLE:
            case Value::Type::FLOAT:
            {
                std::array val = { value.asFloat() };
                m_scriptCompGroup->createWidget<Drag<float>>(key, ImGuiDataType_Float, val)->getOnDataChangedEvent().addListener([key, this](auto& val) {                   
                    auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                    scriptComp->onMemberValueChanged(key, Value(val[0]));
                });
            }
            break;

            case Value::Type::BOOLEAN:
            {
                m_scriptCompGroup->createWidget<CheckBox>(key, value.asBool())->getOnDataChangedEvent().addListener([key, this](bool val) {
                    auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                    scriptComp->onMemberValueChanged(key, Value(val));
                });
            }
            break;

            case Value::Type::STRING:
            {
                json jVal = json::parse(value.asString(), 0, false);
                auto uuid = jVal.is_null() || jVal.is_discarded() ? "" : jVal.value("uuid", std::string());
                auto compName = jVal.is_null() || jVal.is_discarded() ? "" : jVal.value("comp", std::string());
                auto sceneObject = Editor::getCurrentScene()->findObjectByUUID(uuid);
                auto txtField = m_scriptCompGroup->createWidget<TextField>(key, sceneObject ? (compName.empty() ? sceneObject->getName() : sceneObject->getName() + "/" + compName) : value.asString());

                if (sceneObject)
                {
                    auto id = sceneObject ? sceneObject->getId() : (uint64_t)-1;
                    txtField->getOnHoveredEvent().addListener([id, this](auto widget) {
                        if(widget->isHovered()) Editor::getCanvas()->getHierarchy()->setNodeHighlight(id, true);
                    });

                    auto objUUID = sceneObject->getUUID();
                    auto createMenu = m_scriptCompGroup->getPlugin<ContextMenu>();
                    if (createMenu)
                        createMenu->removeAllWidgets();
                    else
                        createMenu = m_scriptCompGroup->addPlugin<ContextMenu>("Component_Select_Menu");

                    createMenu->createWidget<MenuItem>("SceneObject")->getOnClickEvent().addListener([key, objUUID, this](auto widget) {
                        auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                        json jVal = json{
                                {"uuid", objUUID},
                                {"comp", std::string()},
                        };
                        scriptComp->onMemberValueChanged(key, Value(jVal.dump()));
                        dirty();
                    });

                    for (const auto& comp : sceneObject->getComponents())
                    {
                        if (!comp->isSkipSerialize())
                        {
                            auto compName = comp->getName();
                            createMenu->createWidget<MenuItem>(comp->getName())->getOnClickEvent().addListener([key, objUUID, compName, this](auto widget) {
                                auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                                json jVal = json{
                                        {"uuid", objUUID},
                                        {"comp", compName},
                                };
                                scriptComp->onMemberValueChanged(key, Value(jVal.dump()));
                                dirty();
                            });
                        }
                    }
                }

                txtField->addPlugin<DDTargetPlugin<uint64_t>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([key, this](auto val) {
                    auto obj = m_targetObject->getScene()->findObjectById(val);
                    if (obj)
                    {
                        auto objUUID = obj->getUUID();
                        auto createMenu = m_scriptCompGroup->getPlugin<ContextMenu>();
                        if (createMenu)
                            createMenu->removeAllWidgets();
                        else 
                            createMenu = m_scriptCompGroup->addPlugin<ContextMenu>("Component_Select_Menu");
                        createMenu->createWidget<MenuItem>("SceneObject")->getOnClickEvent().addListener([key, objUUID, this](auto widget) {
                            auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                            json jVal = json{
                                    {"uuid", objUUID},
                                    {"comp", std::string()},
                            };
                            scriptComp->onMemberValueChanged(key, Value(jVal.dump()));
                            dirty();
                        });

                        for (const auto& comp : obj->getComponents())
                        {
                            if (!comp->isSkipSerialize())
                            {
                                auto compName = comp->getName();
                                createMenu->createWidget<MenuItem>(comp->getName())->getOnClickEvent().addListener([key, objUUID, compName, this](auto widget) {
                                    auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                                    json jVal = json{
                                            {"uuid", objUUID},
                                            {"comp", compName},
                                    };
                                    scriptComp->onMemberValueChanged(key, Value(jVal.dump()));
                                    dirty();
                                });
                            }
                        }
                        createMenu->open();
                    }
                });
               
                txtField->addPlugin<DDTargetPlugin<std::string>>(EDragDropID::FILE)->getOnDataReceivedEvent().addListener([key, this](auto val) {
                    auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                    scriptComp->onMemberValueChanged(key, Value(val));
                    dirty();
                });

                for (auto ext : AllFileExts)
                {
                    for (const auto& type : GetFileExtensionSuported(ext))
                    {
                        txtField->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([key, this](auto val) {
                            auto scriptComp = m_targetObject->getComponent<ScriptComponent>();
                            scriptComp->onMemberValueChanged(key, Value(val));
                            dirty();
                        });
                    }
                }
            }
            break;
        }
    }
}
NS_IGE_END