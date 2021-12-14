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
#include "core/filesystem/FileSystem.h"

#include <utils/Serialize.h>
#include <imgui.h>

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

        auto prjPath = fs::path(Editor::getInstance()->getProjectPath());
        auto prjName = prjPath.stem().string();
        auto prjFile = prjPath.append(prjName + ".igeproj");

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

        auto sceneGroup = createWidget<Group>("Scene Manager");
        auto startScene = sceneGroup->createWidget<TextField>("Start scene", getStartScene().c_str(), true);
        startScene->addPlugin<DDTargetPlugin<std::string>>(".scene")->getOnDataReceivedEvent().addListener([this](const auto& path) {
            setStartScene(path);
            saveSettings();
            redraw();
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
        };

        auto prjPath = fs::path(Editor::getInstance()->getProjectPath());
        auto prjName = prjPath.stem().string();
        auto prjFile = prjPath.append(prjName + ".igeproj");

        std::ofstream file(prjFile);
        file << settingsJson;
        file.close();
    }
}
