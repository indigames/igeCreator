#include <imgui.h>

#include "core/menu/MenuBar.h"
#include "core/menu/Menu.h"
#include "core/menu/MenuItem.h"
#include "core/widgets/Button.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/dialog/OpenFileDialog.h"
#include "core/dialog/SaveFileDialog.h"
#include "core/panels/Hierarchy.h"
#include "core/panels/EditorScene.h"
#include "core/task/TaskManager.h"

namespace ige::creator
{
    MenuBar::MenuBar(const std::string& name, bool enable)
        : Widget(enable), m_name(name)
    {
    }

    MenuBar::~MenuBar()
    {

    }

    void MenuBar::initialize()
    {
        createFileMenu();
    }

    void MenuBar::_drawImpl()
    {
        static bool initialized = false;
        if (!initialized)
        {
            initialize();
            initialized = true;
        }

        if (m_widgets.empty()) return;

        if(ImGui::BeginMainMenuBar())
        {
            drawWidgets();

            float offset = ImGui::GetWindowHeight();
            ImGui::EndMainMenuBar();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset);
        }
    }

    void MenuBar::createFileMenu()
    {
        auto fileMenu = createWidget<Menu>("File");
        fileMenu->createWidget<MenuItem>("Open Project", "CTRL + Shift + O")->getOnClickEvent().addListener([this](auto widget) {
            auto path = OpenFolderDialog("Path", ".", OpenFileDialog::Option::force_path).result();
            if (!path.empty())
            {
                TaskManager::getInstance()->addTask([path]() {
                    Editor::getInstance()->openProject(path);
                });
            }
        });

        fileMenu->createWidget<MenuItem>("Open Scene", "CTRL + O")->getOnClickEvent().addListener([this](auto widget){
            auto selectedFiles = OpenFileDialog("Open", ".", { "scene", "*.scene" }).result();
            if (!selectedFiles.empty() && !selectedFiles[0].empty())
            {
                auto sceneFile = selectedFiles[0];
                TaskManager::getInstance()->addTask([sceneFile](){
                    Editor::getInstance()->loadScene(sceneFile);
                });
            }
        });

        fileMenu->createWidget<MenuItem>("Save Scene", "CTRL + S")->getOnClickEvent().addListener([this](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->saveScene();
            });
        });

        fileMenu->createWidget<MenuItem>("Exit", "ALT + F4")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getApp()->quit();
            });
        });

        auto viewMenu = createWidget<Menu>("View");
        viewMenu->createWidget<MenuItem>("Fullscreen", "F11")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->toggleFullScreen();
            });
        });

        viewMenu->createWidget<MenuItem>("Reset")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->resetLayout();
            });
        });

        auto buildMenu = createWidget<Menu>("Build");
        buildMenu->createWidget<MenuItem>("Convert Assets")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->convertAssets();
            });
        });
        buildMenu->createWidget<MenuItem>("ROM")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildRom();
            });
        });
        buildMenu->createWidget<MenuItem>("Windows")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildPC();
            });
        });
        buildMenu->createWidget<MenuItem>("Android")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildAndroid();
            });
        });
        buildMenu->createWidget<MenuItem>("iOS")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildIOS();
            });
        });

        auto helpMenu = createWidget<Menu>("Help");
        helpMenu->createWidget<MenuItem>("Document", "CTRL + F1 ")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->openDocument();
            });
        });
        helpMenu->createWidget<MenuItem>("About")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->openAbout();
            });
        });
    }
}
