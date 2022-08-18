#include "core/panels/ProjectSetting.h"
#include "core/layout/Group.h"
#include "core/layout/Columns.h"
#include "core/widgets/Label.h"
#include "core/widgets/TextField.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Button.h"
#include "core/widgets/CheckBox.h"
#include "core/widgets/Drag.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/Editor.h"
#include "core/filesystem/FileSystem.h"

#include <utils/Serialize.h>
#include <imgui.h>
#include <sstream>
#include <iomanip>

namespace ige::creator
{
    ProjectSetting::ProjectSetting(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
    }

    ProjectSetting::~ProjectSetting()
    {
        clear();
    }

    void ProjectSetting::initialize()
    {
        clear();

        json settingsJson;
        auto prjFile = fs::path(Editor::getInstance()->getProjectPath()).append(Editor::getInstance()->getProjectFileName());

        std::ifstream file(prjFile);
        if (file.is_open()) {
            file >> settingsJson;
            file.close();
        }

        if (!settingsJson.is_null())
        {
            from_json(settingsJson, *this);
        }

        drawSettings();
    }

    //! Draw settings
    void  ProjectSetting::drawSettings(){
        removeAllWidgets();

        createWidget<TextField>("Name", appName.c_str())->getOnDataChangedEvent().addListener([this](const auto& txt) {
            appName = txt;
        });
        createWidget<TextField>("Label", appLabel.c_str())->getOnDataChangedEvent().addListener([this](const auto& txt) {
            appLabel = txt;
        });
        createWidget<TextField>("VersionName", versionName.c_str())->getOnDataChangedEvent().addListener([this](const auto& txt) {
            versionName = txt;
        });
        auto vals = std::array{ (float)versionCode };
        createWidget<Drag<float>>("VersionCode", ImGuiDataType_S32, vals, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
            versionCode = val[0];
        });
        createWidget<TextField>("BundleId", bundleId.c_str())->getOnDataChangedEvent().addListener([this](const auto& txt) {
            bundleId = txt;
        });
        createWidget<TextField>("Orientation", orientation.c_str())->getOnDataChangedEvent().addListener([this](const auto& txt) {
            orientation = txt;
        });
        auto txt_startScene = createWidget<TextField>("StartScene", startScene.c_str());
        txt_startScene->getOnDataChangedEvent().addListener([this](const auto& txt) {
            startScene = txt;
        });
        txt_startScene->addPlugin<DDTargetPlugin<std::string>>(".scene")->getOnDataReceivedEvent().addListener([this](const auto& path) {
            startScene = path;
        });
        auto btnSave = createWidget<Button>("Save", ImVec2(0, 0))->getOnClickEvent().addListener([this](const auto& widget) {
            saveSettings();
        });
        auto dependGroup = createWidget<Group>("Dependencies");
        auto columns = dependGroup->createWidget<Columns<2>>();
        for (const auto& [k, v] : dependencies) {
            auto key = k;
            columns->createWidget<CheckBox>(k, v)->getOnDataChangedEvent().addListener([key, this](auto val) {
                dependencies[key] = val;
            });
        }
        auto platformsGroup = createWidget<Group>("Platforms");
        for (const auto& platform : platforms) {
            auto platformGroup = platformsGroup->createWidget<Group>(platform->platform.c_str());
            platformGroup->createWidget<TextField>("RomDir", platform->romDir.c_str())->getOnDataChangedEvent().addListener([platform, this](const auto& txt) {
                platform->romDir = txt;
            });
            platformGroup->createWidget<TextField>("ConfigDir", platform->configDir.c_str())->getOnDataChangedEvent().addListener([platform, this](const auto& txt) {
                platform->configDir = txt;
            });
            std::stringstream platforms_str;
            for (const auto& arch : platform->archs) {
                platforms_str << arch << ";";
            }
            platformGroup->createWidget<TextField>("Archs", platforms_str.str().c_str())->getOnDataChangedEvent().addListener([platform, this](const auto& txt) {
                std::istringstream f(txt);
                std::string s;
                platform->archs.clear();
                while (std::getline(f, s, ';')) {
                    platform->archs.push_back(s);
                }
            });
            auto android = std::dynamic_pointer_cast<AndroidSetting>(platform);
            if (android) {
                auto vals = std::array{ (float)android->minSdk };
                platformGroup->createWidget<Drag<float>>("MinSdkVersion", ImGuiDataType_S32, vals, 1, 0)->getOnDataChangedEvent().addListener([android, this](auto val) {
                    android->minSdk = val[0];
                });
                auto vals2 = std::array{ (float)android->targetSdk };
                platformGroup->createWidget<Drag<float>>("TargetSdkVersion", ImGuiDataType_S32, vals2, 1, 0)->getOnDataChangedEvent().addListener([android, this](auto val) {
                    android->targetSdk = val[0];
                });
                auto permissionGroup = platformGroup->createWidget<Group>("Permissions");
                auto permColumns = permissionGroup->createWidget<Columns<2>>();
                permColumns->setColumnWidth(1, 32.f);
                for (const auto& perm : android->permissions) {
                    auto permission = perm;
                    permColumns->createWidget<TextField>("", perm.c_str(), true);
                    permColumns->createWidget<Button>("-", ImVec2(ImGui::GetFrameHeight(), 0))->getOnClickEvent().addListener([this, android, permission](const auto& widget) {
                        auto itr = std::find(android->permissions.begin(), android->permissions.end(), permission);
                        if (itr != android->permissions.end()) {
                            android->permissions.erase(itr);
                            redraw();
                        }
                    });
                }
                static auto s_addingPermission = std::string();
                permColumns->createWidget<TextField>("", s_addingPermission.c_str())->getOnDataChangedEvent().addListener([](const auto& val) {
                    s_addingPermission = val;
                });
                permColumns->createWidget<Button>("+", ImVec2(ImGui::GetFrameHeight(), 0))->getOnClickEvent().addListener([this, android](const auto& widget) {
                    auto itr = std::find(android->permissions.begin(), android->permissions.end(), s_addingPermission);
                    if (itr == android->permissions.end()) {
                        android->permissions.push_back(s_addingPermission);
                        redraw();
                    }
                });
                auto featureGroup = platformGroup->createWidget<Group>("Features");
                auto columns = featureGroup->createWidget<Columns<3>>();
                columns->setColumnWidth(1, 96.f);
                columns->setColumnWidth(2, 32.f);
                columns->createWidget<TextField>("", "Feature", false);
                columns->createWidget<TextField>("", "Required", false);
                columns->createWidget<TextField>("", "", false);
                for (const auto& [feature, require]: android->features) {
                    auto featureStr = feature;
                    columns->createWidget<TextField>("", feature.c_str(), true);
                    columns->createWidget<CheckBox>("", require)->getOnDataChangedEvent().addListener([android, featureStr](auto val) {
                        android->features[featureStr] = val;
                    });
                    columns->createWidget<Button>("-", ImVec2(ImGui::GetFrameHeight(), 0))->getOnClickEvent().addListener([this, android, featureStr](const auto& widget) {
                        auto itr = android->features.find(featureStr);
                        if (itr != android->features.end()) {
                            android->features.erase(itr);
                            redraw();
                        }
                    });
                }
                static auto s_addingFeature = std::string();
                columns->createWidget<TextField>("", s_addingFeature.c_str())->getOnDataChangedEvent().addListener([](const auto& val) {
                    s_addingFeature = val;
                });
                static auto s_addingRequire = false;
                columns->createWidget<CheckBox>("", s_addingRequire)->getOnDataChangedEvent().addListener([](auto val) {
                    s_addingRequire = val;
                });
                columns->createWidget<Button>("+", ImVec2(ImGui::GetFrameHeight(), 0))->getOnClickEvent().addListener([this, android](const auto& widget) {
                    auto itr = android->features.find(s_addingFeature);
                    if (itr == android->features.end()) {
                        android->features[s_addingFeature] = s_addingRequire;
                        redraw();
                    }
                });
            }
            auto ios = std::dynamic_pointer_cast<IOSSetting>(platform);
            if (ios) {
                platformGroup->createWidget<TextField>("DeploymentTarget", ios->deployTarget.c_str())->getOnDataChangedEvent().addListener([ios, this](const auto& txt) {
                    ios->deployTarget = txt;
                });
                platformGroup->createWidget<TextField>("DeviveFamily", ios->deviceFamily.c_str())->getOnDataChangedEvent().addListener([ios, this](const auto& txt) {
                    ios->deviceFamily = txt;
                });
                platformGroup->createWidget<TextField>("DevelopmentTeamID", ios->developerTeamId.c_str())->getOnDataChangedEvent().addListener([ios, this](const auto& txt) {
                    ios->developerTeamId = txt;
                });
                platformGroup->createWidget<TextField>("CodeSignIdentity", ios->codeSignIdentity.c_str())->getOnDataChangedEvent().addListener([ios, this](const auto& txt) {
                    ios->codeSignIdentity = txt;
                });
                platformGroup->createWidget<TextField>("ProvisioningProfile", ios->provisionProfile.c_str())->getOnDataChangedEvent().addListener([ios, this](const auto& txt) {
                    ios->provisionProfile = txt;
                });
            }
        }
    }

    void ProjectSetting::_drawImpl()
    {
        if (m_bNeedRedraw)
        {
            drawSettings();
            m_bNeedRedraw = false;
            return;
        }

        Panel::_drawImpl();
    }

    void ProjectSetting::clear()
    {
        removeAllWidgets();
        platforms.clear();
    }

    void ProjectSetting::saveSettings()
    {
        json settingsJson;
        to_json(settingsJson, *this);

        auto prjPath = fs::path(Editor::getInstance()->getProjectPath()).append(Editor::getInstance()->getProjectFileName());
        std::ofstream file(prjPath);
        file << std::setw(2) << settingsJson << std::endl;
        file.close();
    }

    //! Serialize
    void to_json(json& j, const ProjectSetting& obj) {
        j["Name"] = obj.appName;
        j["Label"] = obj.appLabel;
        j["VersionName"] = obj.versionName;
        j["VersionCode"] = obj.versionCode;
        j["BundleId"] = obj.bundleId;
        j["Orientation"] = obj.orientation;
        j["StartScene"] = obj.startScene;
        j["Requires"] = obj.dependencies;

        auto jPlatforms = json::array();
        for (const auto& platform : obj.platforms) {
            json jPlatform;
            platform->to_json(jPlatform);
            jPlatforms.push_back(jPlatform);
        }
        j["Platforms"] = jPlatforms;
    }

    //! Deserialize
    void from_json(const json& j, ProjectSetting& obj) {
        obj.appName = j.value("Name", "Sample");
        obj.appLabel = j.value("Label", "Sample");
        obj.versionName = j.value("VersionName", "0.0.1");
        obj.versionCode = j.value("VersionCode", 1);
        obj.bundleId = j.value("BundleId", "com.example.app");
        obj.orientation = j.value("Orientation", "portrait");
        obj.startScene = j.value("StartScene", "scenes/main.scene");
        obj.dependencies = j.value("Requires", std::map<std::string, bool>());

        auto jPlatforms = j.at("Platforms");        
        for (const auto& jPlatform: jPlatforms) {
            auto platform = jPlatform.value("Platform", "windows");
            if (platform.compare("android") == 0) {
                auto setting = std::make_shared<AndroidSetting>();
                setting->from_json(jPlatform);
                obj.platforms.push_back(setting);
            }
            else if (platform.compare("ios") == 0) {
                auto setting = std::make_shared<IOSSetting>();
                setting->from_json(jPlatform);
                obj.platforms.push_back(setting);
            }
            else {
                auto setting = std::make_shared<PlatformSetting>();
                setting->from_json(jPlatform);
                obj.platforms.push_back(setting);
            }
        }
    }

    //! Serialize component
    void to_json(json& j, const PlatformSetting &obj)
    {
        obj.to_json(j);
    }

    //! Deserialize component
    void from_json(const json& j, PlatformSetting& obj)
    {
        obj.from_json(j);
    }
}
