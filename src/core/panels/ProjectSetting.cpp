#include <imgui.h>

#include "core/panels/ProjectSetting.h"

#include "core/layout/Group.h"
#include "core/layout/Columns.h"
#include "core/widgets/Label.h"
#include "core/widgets/TextField.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Button.h"
#include "core/widgets/Drag.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/Editor.h"

#include <utils/Serialize.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::creator
{
    ProjectSetting::ProjectSetting(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        json settingsJson;

        auto prjPath = fs::path(Editor::getInstance()->getProjectPath());
        auto prjName = prjPath.stem().string();
        auto prjFile = prjPath.append(prjName + ".igeproj");

        if (!fs::exists(prjFile))
        {
            json settingsJson = json{
                {"startScene", "scenes/main.scene"},
            };

            std::ofstream file(prjFile.string());
            file << settingsJson;
            file.close();
        }

        std::ifstream file(prjFile);
        if (file.is_open())
        {
            file >> settingsJson;
            file.close();
            setStartScene(settingsJson.value("startScene", "scenes/main.scene"));
        }

        if (!settingsJson.is_null())
        {
            setStartScene(settingsJson.value("startScene", "scenes/main.scene"));
        }
            
    }

    ProjectSetting::~ProjectSetting()
    {
        clear();
    }

    void ProjectSetting::initialize()
    {
        clear();
        
        auto sceneGroup = createWidget<Group>("Scene Manager");
        auto startScene = sceneGroup->createWidget<TextField>("Start scene", getStartScene().c_str(), true);
        startScene->addPlugin<DDTargetPlugin<std::string>>(".scene")->getOnDataReceivedEvent().addListener([this](auto val) {
            setStartScene(val);
            redraw();
        });

        createWidget<Separator>();
        auto saveBtn = createWidget<Button>("Save", ImVec2(64.f, 0.f));
        saveBtn->getOnClickEvent().addListener([this](auto widget) {
            saveSettings();
        });
    }

    void ProjectSetting::_drawImpl()
    {
        if (m_bNeedRedraw)
        {
            initialize();
            m_bNeedRedraw = false;
            return;
        }

        Panel::_drawImpl();
    }

    void ProjectSetting::clear()
    {
        removeAllWidgets();
    }

    void ProjectSetting::setStartScene(const std::string& path)
    {
        auto fsPath = fs::path(path);
        auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');
        if(m_startScene != relPath)
            m_startScene = relPath;
    }

    void ProjectSetting::saveSettings()
    {
        json settingsJson = json{
            {"startScene", getStartScene()},
            {"gravity", getGravity()},
            {"globalVolume", getGlobalVolume()},
        };

        auto prjPath = fs::path(Editor::getInstance()->getProjectPath());
        auto prjName = prjPath.stem().string();
        auto prjFile = prjPath.append(prjName + ".igeproj");

        std::ofstream file(prjFile);
        file << settingsJson;
        file.close();
    }
}
