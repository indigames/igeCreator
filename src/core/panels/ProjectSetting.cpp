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

#include <utils/Serialize.h>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::creator
{
    ProjectSetting::ProjectSetting(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        json settingsJson;
        auto fsPath = fs::path("settings.json");

        std::ifstream file(fsPath);
        if (file.is_open())
        {
            file >> settingsJson;
            setStartScene(settingsJson.value("startScene", "scene/main.scene"));
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

        auto fsPath = fs::path("settings.json");
        std::ofstream file(fsPath.string());
        file << settingsJson;
    }
}
