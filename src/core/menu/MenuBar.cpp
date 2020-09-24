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
        fileMenu->createWidget<MenuItem>("New Scene", "CTRL + N")->getOnClickEvent().addListener([this](auto widget) {            
            TaskManager::getInstance()->addTask([](){
                Editor::getInstance()->setSelectedObject(-1);
                Editor::getCanvas()->getEditorScene()->clear();

                auto scene = Editor::getCurrentScene();
                if (scene) Editor::getSceneManager()->unloadScene(scene);
                scene = Editor::getSceneManager()->createScene("New scene");
                Editor::setCurrentScene(scene);
            });
        });

        fileMenu->createWidget<MenuItem>("Open Scene", "CTRL + O")->getOnClickEvent().addListener([this](auto widget){
            auto selectedFiles = OpenFileDialog("Open", ".", { "json", "*.json" }).result();
            if (!selectedFiles.empty() && !selectedFiles[0].empty())
            {
                TaskManager::getInstance()->addTask([selectedFiles](){
                    Editor::getInstance()->setSelectedObject(-1);

                    auto scene = Editor::getCurrentScene();
                    if (scene) Editor::getSceneManager()->unloadScene(scene);
                    scene = nullptr;
                    Editor::getCanvas()->getHierarchy()->clear();
                    Editor::getCanvas()->getHierarchy()->initialize();
                    Editor::getCanvas()->getEditorScene()->clear();
                    scene = Editor::getSceneManager()->loadScene(selectedFiles[0]);
                    Editor::setCurrentScene(scene);
                });
            }
        });

        fileMenu->createWidget<MenuItem>("Save Scene", "CTRL + S")->getOnClickEvent().addListener([this](auto widget) {
            auto selectedFile = SaveFileDialog("Save", ".", { "json", "*.json" }).result();
                if (!selectedFile.empty())
                {
                    TaskManager::getInstance()->addTask([selectedFile, this]() {
                        Editor::getSceneManager()->saveScene(selectedFile);
                    });
                }
            });

        fileMenu->createWidget<MenuItem>("Exit", "ALT + F4")->getOnClickEvent().addListener([](auto widget) {
            Editor::getApp()->quit();
        });
    }
}