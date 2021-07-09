#include "core/scene/components/ScriptEditorComponent.h"
#include "core/scene/CompoundComponent.h"

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

ScriptEditorComponent::~ScriptEditorComponent() {
    if (m_watchId != 0) {
        fs::watcher::unwatch(m_watchId);
        m_watchId = 0;
    }
    m_scriptCompGroup = nullptr;
}

void ScriptEditorComponent::onInspectorUpdate() {
    drawScriptComponent();
}

void ScriptEditorComponent::drawScriptComponent() {
    if (m_scriptCompGroup == nullptr) {
        m_scriptCompGroup = m_group->createWidget<Group>("ScriptGroup", false);
    }
    m_scriptCompGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtPath = m_scriptCompGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""));
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](const auto& txt) {
        auto comp = getComponent<CompoundComponent>();
        comp->setProperty("path", txt);
        comp->setDirty();
        setDirty();
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Script)) {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto comp = getComponent<CompoundComponent>();
            comp->setProperty("path", txt);
            comp->setDirty();
            setDirty();
        });
    }
    m_scriptCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Script (*.py)", "*.py" }).result();
        if (files.size() > 0)
        {
            auto comp = getComponent<CompoundComponent>();
            comp->setProperty("path", files[0]);
            comp->setDirty();
            setDirty();
        }
    });

    if (m_watchId != 0)
    {
        fs::watcher::unwatch(m_watchId);
        m_watchId = 0;
    }

    auto path = comp->getProperty<std::string>("path", "");
    if (path.length() > 0)
    {
        m_watchId = fs::watcher::watch(fs::path(path), false, false, std::chrono::milliseconds(1000), [this](const auto&, bool) {
            auto comp = getComponent<CompoundComponent>();
            comp->setProperty("path", comp->getProperty<std::string>("path", ""));
            comp->setDirty();
            setDirty();
        });
    }

    auto jMembers = comp->getProperty<json>("members", json{});
    auto members = jMembers.get<std::unordered_map<std::string, json>>();

    for (const auto& [k, value]: members)
    {
        const auto& key = k;
        switch (value.type())  
        {
        case json::value_t::number_integer:
        case json::value_t::number_unsigned:
            {
                std::array val = { value.get<int>() };
                m_scriptCompGroup->createWidget<Drag<int>>(key, ImGuiDataType_S32, val)->getOnDataChangedEvent().addListener([key, this](auto& val) {                    
                    auto comp = getComponent<CompoundComponent>();
                    auto members = comp->getProperty<json>("members", json::array());
                    if (members.contains(key)) {
                        members[key] = val[0];
                        comp->setProperty("members", members);
                    }
                });
            }
            break;

            case json::value_t::number_float:
            {
                std::array val = { value.get<float>() };
                m_scriptCompGroup->createWidget<Drag<float>>(key, ImGuiDataType_Float, val)->getOnDataChangedEvent().addListener([key, this](auto& val) {                   
                    auto comp = getComponent<CompoundComponent>();
                    auto members = comp->getProperty<json>("members", json::array());
                    if (members.contains(key)) {
                        members[key] = val[0];
                        comp->setProperty("members", members);
                    }
                });
            }
            break;

            case json::value_t::boolean:
            {
                m_scriptCompGroup->createWidget<CheckBox>(key, value.get<bool>())->getOnDataChangedEvent().addListener([key, this](bool val) {
                    auto comp = getComponent<CompoundComponent>();
                    auto members = comp->getProperty<json>("members", json::array());
                    if (members.contains(key)) {
                        members[key] = val;
                        comp->setProperty("members", members);
                    }
                });
            }
            break;

            case json::value_t::string:
            {
                auto jVal = json::parse(value.get<std::string>(), 0, false);
                auto uuid = jVal.is_null() || jVal.is_discarded() ? "" : jVal.value("uuid", std::string());
                auto compName = jVal.is_null() || jVal.is_discarded() ? "" : jVal.value("comp", std::string());
                auto sceneObject = Editor::getCurrentScene()->findObjectByUUID(uuid);
                auto txtField = m_scriptCompGroup->createWidget<TextField>(key, sceneObject ? (compName.empty() ? sceneObject->getName() : sceneObject->getName() + "/" + compName) : value.get<std::string>());

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
                        json jVal = json{
                                {"uuid", objUUID},
                                {"comp", std::string()},
                        };
                        auto comp = getComponent<CompoundComponent>();
                        auto members = comp->getProperty<json>("members", json::array());
                        if (members.contains(key)) {
                            members[key] = jVal;
                            comp->setProperty("members", members);
                        }
                        setDirty();
                    });

                    for (const auto& comp : sceneObject->getComponents())
                    {
                        if (!comp->isSkipSerialize())
                        {
                            auto compName = comp->getName();
                            createMenu->createWidget<MenuItem>(comp->getName())->getOnClickEvent().addListener([key, objUUID, compName, this](auto widget) {
                                json jVal = json{
                                        {"uuid", objUUID},
                                        {"comp", compName},
                                };
                                auto comp = getComponent<CompoundComponent>();
                                auto members = comp->getProperty<json>("members", json::array());
                                if (members.contains(key)) {
                                    members[key] = jVal;
                                    comp->setProperty("members", members);
                                }
                                setDirty();
                            });
                        }
                    }
                }

                txtField->addPlugin<DDTargetPlugin<uint64_t>>(EDragDropID::OBJECT)->getOnDataReceivedEvent().addListener([key, this](auto val) {
                    auto obj = getComponent()->getOwner()->getScene()->findObjectById(val);
                    if (obj)
                    {
                        auto objUUID = obj->getUUID();
                        auto createMenu = m_scriptCompGroup->getPlugin<ContextMenu>();
                        if (createMenu)
                            createMenu->removeAllWidgets();
                        else 
                            createMenu = m_scriptCompGroup->addPlugin<ContextMenu>("Component_Select_Menu");
                        createMenu->createWidget<MenuItem>("SceneObject")->getOnClickEvent().addListener([key, objUUID, this](auto widget) {
                            json jVal = json{
                                    {"uuid", objUUID},
                                    {"comp", std::string()},
                            };
                            auto comp = getComponent<CompoundComponent>();
                            auto members = comp->getProperty<json>("members", json::array());
                            if (members.contains(key)) {
                                members[key] = jVal;
                                comp->setProperty("members", members);
                            }
                            setDirty();
                        });

                        for (const auto& comp : obj->getComponents())
                        {
                            if (!comp->isSkipSerialize())
                            {
                                auto compName = comp->getName();
                                createMenu->createWidget<MenuItem>(comp->getName())->getOnClickEvent().addListener([key, objUUID, compName, this](auto widget) {
                                    json jVal = json{
                                            {"uuid", objUUID},
                                            {"comp", compName},
                                    };
                                    auto comp = getComponent<CompoundComponent>();
                                    auto members = comp->getProperty<json>("members", json::array());
                                    if (members.contains(key)) {
                                        members[key] = jVal;
                                        comp->setProperty("members", members);
                                    }
                                    setDirty();
                                });
                            }
                        }
                        createMenu->open();
                    }
                });
               
                txtField->addPlugin<DDTargetPlugin<std::string>>(EDragDropID::FILE)->getOnDataReceivedEvent().addListener([key, this](auto val) {
                    auto comp = getComponent<CompoundComponent>();
                    auto members = comp->getProperty<json>("members", json::array());
                    if (members.contains(key)) {
                        members[key] = val;
                        comp->setProperty("members", members);
                    }
                    setDirty();
                });

                for (auto ext : AllFileExts)
                {
                    for (const auto& type : GetFileExtensionSuported(ext))
                    {
                        txtField->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([key, this](auto val) {
                            auto comp = getComponent<CompoundComponent>();
                            auto members = comp->getProperty<json>("members", json::array());
                            if (members.contains(key)) {
                                members[key] = val;
                                comp->setProperty("members", members);
                            }
                            setDirty();
                        });
                    }
                }
            }
            break;
        }
    }
}
NS_IGE_END