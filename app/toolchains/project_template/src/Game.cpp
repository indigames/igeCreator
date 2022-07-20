#ifdef USE_IGESCENE

#include "Game.h"

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include <scene/SceneManager.h>
#include <scene/Scene.h>
using namespace ige::scene;

Game::Game()
{
}

Game::~Game()
{
}

bool Game::init()
{
    if(m_inputProcessor == nullptr) {
        // Init input processor
        m_inputProcessor = std::make_shared<InputProcessor>();

        // Set project path
        SceneManager::getInstance()->setProjectPath(pyxieFios::Instance().GetRoot());

        // Load scene using scene manager
        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            if (entry.is_regular_file()) {
                if(entry.path().extension().compare(".igeproj") == 0) {
                    std::ifstream file(entry.path());
                    std::string scenePath = "";
                    if (file.is_open()) {
                        json jScene;
                        file >> jScene;
                        file.close();
                        scenePath = jScene.value("startScene", "scene/main.scene");
                        if (fs::exists(scenePath)) {
                            auto scene = SceneManager::getInstance()->createScene();
                            SceneManager::getInstance()->loadScene(scene, scenePath);
                            SceneManager::getInstance()->setCurrentScene(scene);
                            if (scene->getCanvas()) {
                                const auto& size = scene->getCanvas()->getDesignCanvasSize();
                                showAppWindow(true, size.X(), size.Y());
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    return true;
}

void Game::onSize(int w, int h) {
    Application::onSize(w, h);
    if (SceneManager::hasInstance()
        && SceneManager::getInstance()->getCurrentScene()) {
        auto size = Vec2(w, h);
        SceneManager::getInstance()->getCurrentScene()->setWindowSize(size);
        SceneManager::getInstance()->getCurrentScene()->setViewSize(size);
        if (SceneManager::getInstance()->getCurrentScene()->getCanvas()) {
            SceneManager::getInstance()->getCurrentScene()->getCanvas()->setTargetCanvasSize(size);
        }
    }
}

bool Game::onUpdate()
{
    // Update super
    Application::onUpdate();

    // Ensure initialization
    init();

    // Update scene
    auto dt = (float)Time::Instance().GetElapsedTime();

    //! Update touch
    updateTouch();

    SceneManager::getInstance()->update(dt);

    // Update physic
    SceneManager::getInstance()->physicUpdate(dt);

    // Late Update
    SceneManager::getInstance()->lateUpdate(dt);

    return true;
}

void Game::onRender()
{
    // Render super
    Application::onRender();


    // Render scene
    SceneManager::getInstance()->render();
}

void Game::onShutdown()
{
    m_inputProcessor = nullptr;

    SceneManager::destroy();

    // Destroy super
    Application::onShutdown();
}

void Game::updateTouch()
{
    auto touch = getInputHandler()->getTouchDevice();
    for (int i = 0; i < touch->getFingersCount(); ++i) {
        auto fingerId = touch->getFinger(i)->getFingerId();
        if (touch->isFingerPressed(i)) {
            float touchX, touchY;
            touch->getFingerPosition(i, touchX, touchY);
            m_inputProcessor->touchDown(fingerId, touchX, touchY);
        }
        if (touch->isFingerMoved(i)) {
            float touchX, touchY;
            touch->getFingerPosition(i, touchX, touchY);
            m_inputProcessor->touchMove(fingerId, touchX, touchY);
        }
        if (touch->isFingerReleased(i)) {
            float touchX, touchY;
            touch->getFingerPosition(i, touchX, touchY);
            m_inputProcessor->touchUp(fingerId, touchX, touchY);
        }
    }
}
#endif
