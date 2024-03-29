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

#include <imgui.h>

namespace ige::creator
{
    MenuBar::MenuBar(const std::string& name, bool enable)
        : Widget(enable), m_name(name)
    {
        initialize();
    }

    MenuBar::~MenuBar()
    {
    }

    void MenuBar::initialize()
    {
        removeAllPlugins();
        removeAllWidgets();
        createFileMenu();
    }

    void MenuBar::_drawImpl()
    {
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
        auto projectOpened = (Editor::getInstance()->getProjectPath().length() > 0);
        auto isPrefabScene = Editor::getCurrentScene() && Editor::getCurrentScene()->isPrefab();
        auto fileMenu = createWidget<Menu>("File");
        fileMenu->createWidget<MenuItem>("New Project", "CTRL + SHIFT + N")->getOnClickEvent().addListener([this](auto widget) {
            Editor::getInstance()->createProject();
        });
        fileMenu->createWidget<MenuItem>("Open Project", "CTRL + SHIFT + O")->getOnClickEvent().addListener([this](auto widget) {
            Editor::getInstance()->openProject();
        });
        fileMenu->createWidget<MenuItem>("New Scene", "CTRL + ALT + N", projectOpened && !isPrefabScene)->getOnClickEvent().addListener([this](auto widget) {
            TaskManager::getInstance()->addTask([](){
                Editor::getInstance()->createScene();
            });
        });
        fileMenu->createWidget<MenuItem>(isPrefabScene ? "Save Prefab" : "Save Scene", "CTRL + S", projectOpened)->getOnClickEvent().addListener([this](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                if (Editor::getCurrentScene() && Editor::getCurrentScene()->isPrefab()) {
                    Editor::getInstance()->savePrefab();
                }
                else {
                    Editor::getInstance()->saveScene();
                }
            });
        });
        fileMenu->createWidget<MenuItem>(isPrefabScene ? "Save Prefab As" : "Save Scene As", "CTRL + SHIFT + S", projectOpened)->getOnClickEvent().addListener([this](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                if (Editor::getCurrentScene() && Editor::getCurrentScene()->isPrefab()) {
                    Editor::getInstance()->savePrefabAs();
                }
                else {
                    Editor::getInstance()->saveSceneAs();
                }
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

        auto toolMenu = createWidget<Menu>("Tools");
        toolMenu->createWidget<MenuItem>("Bitmap Font Creator")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->toggleBitmapFontCreator();
                });
            });
        toolMenu->createWidget<MenuItem>("Convert Texture")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->convertAssets();
                });
            });

        auto buildMenu = createWidget<Menu>("Build");
#if defined(_WIN32)
        buildMenu->createWidget<MenuItem>("Windows")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildPC();
            });
        });
#elif defined(__APPLE__)
        buildMenu->createWidget<MenuItem>("iOS")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildIOS();
            });
        });
        buildMenu->createWidget<MenuItem>("macOS")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildMacOS();
            });
        });
#endif
        buildMenu->createWidget<MenuItem>("Android")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildAndroid();
            });
        });
        buildMenu->createWidget<MenuItem>("WebGL")->getOnClickEvent().addListener([](auto widget) {
            TaskManager::getInstance()->addTask([]() {
                Editor::getInstance()->buildWebGL();
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
