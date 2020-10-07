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

            setStartScene(settingsJson.value("startScene", "scenes/main.json"));
            setGravity(settingsJson.value("gravity", Vec3(0.f, -9.81f, 0.f)));
            setGlobalVolume(settingsJson.value("globalVolume", 1.f));
        }
    }

    ProjectSetting::~ProjectSetting()
    {

    }

    void ProjectSetting::initialize()
    {
        clear();
        
        auto sceneGroup = createWidget<Group>("Scene Manager");
        auto startScene = sceneGroup->createWidget<TextField>("Start scene", getStartScene().c_str());
        startScene->getOnDataChangedEvent().addListener([this](auto val) {
            setStartScene(val);
        });
        startScene->addPlugin<DDTargetPlugin<std::string>>(".json")->getOnDataReceivedEvent().addListener([this](auto val) {
            setStartScene(val);
            redraw();
        });

        auto physicGroup = createWidget<Group>("Physic Manager");        
        std::array gravity = { m_gravity[0], m_gravity[1], m_gravity[2] };
        physicGroup->createWidget<Drag<float, 3>>("Gravity", ImGuiDataType_Float, gravity, 0.01f)->getOnDataChangedEvent().addListener([this](auto& val) {
            setGravity({ val[0], val[1], val[2] });
        });

        auto audioGroup = createWidget<Group>("Audio Manager");
        std::array volume = { getGlobalVolume() };
        audioGroup->createWidget<Drag<float>>("Global Volume", ImGuiDataType_Float, volume, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
            setGlobalVolume(val[0]);
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
