#include "Game.h"

#include <scene/SceneManager.h>
#include <scene/Scene.h>
using namespace ige::scene;

#include <json/json.hpp>
using json = nlohmann::json;

#include <iostream>
#include <fstream>

Game::Game()
{

}

Game::~Game()
{

}

bool Game::onInit(DeviceHandle dh)
{
    // Init super
    Application::onInit(dh);

    // Load config
    std::ifstream file("settings.json");
	std::string scenePath = "scene/main.scene";

    if (file.is_open())
    {
        json jScene;
        file >> jScene;
        scenePath = jScene.value("startScene", "scene/main.scene");
    }

    // Load scene
    SceneManager::getInstance()->loadScene(scenePath);

    return true;
}

bool Game::onUpdate()
{
    // Update super
    Application::onUpdate();

    auto dt = (float)Time::Instance().GetElapsedTime();

    // Update scene
    SceneManager::getInstance()->update(dt);

    return true;
}

void Game::onRender()
{
    // Render super
    Application::onRender();

    auto renderContext = RenderContext::InstancePtr();
    if (renderContext)
    {
        renderContext->BeginScene(nullptr, { 0.f, 0.f, 0.f, 0.f }, true, true);
        SceneManager::getInstance()->render();
        renderContext->EndScene();
    }
}

void Game::onShutdown()
{
    SceneManager::destroy();

    // Destroy super
    Application::onShutdown();
}
