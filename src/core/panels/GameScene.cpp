#include <imgui.h>

#include <filesystem>
namespace fs = std::filesystem;

#include "core/panels/GameScene.h"
#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Hierarchy.h"
#include "core/panels/EditorScene.h"
#include "core/dialog/SaveFileDialog.h"
#include "core/task/TaskManager.h"

#include <utils/GraphicsHelper.h>
#include <scene/Scene.h>
#include <scene/SceneObject.h>
#include <scene/SceneManager.h>
using namespace ige::scene;

#include <utils/PyxieHeaders.h>
using namespace pyxie;

namespace ige::creator
{
    GameScene::GameScene(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {}

    GameScene::~GameScene()
    {
        clear();

        // Clear the temp scene file if exist
        if (SceneManager::getInstance()->getCurrentScene())
        {
            auto name = SceneManager::getInstance()->getCurrentScene()->getName();
            auto fsPath = fs::path(name + "_tmp");
            auto ext = fsPath.extension();
            if (ext.string() != ".scene")
                fsPath = fsPath.replace_extension(".scene");
            fs::remove(fsPath);
        }        
    }

    void GameScene::clear()
    {   
        m_imageWidget = nullptr;
        m_bInitialized = false;
       
        removeAllWidgets();

        if (m_rtTexture)
        {
            m_rtTexture->DecReference();
            m_rtTexture = nullptr;
        }

        if (m_fbo)
        {
            m_fbo->DecReference();
            m_fbo = nullptr;
        }
    }

    void GameScene::initialize()
    {
        if (!m_bInitialized) {
            auto size = getSize();
            if (size.x > 0 && size.y > 0)
            {
                m_rtTexture = ResourceCreator::Instance().NewTexture("GameScene_RTTexture", nullptr, size.x, size.y, GL_RGBA);
                m_fbo = ResourceCreator::Instance().NewRenderTarget(m_rtTexture, true, true);
                m_imageWidget = createWidget<Image>(m_fbo->GetColorTexture()->GetTextureHandle(), size);

                // Position changed event
                getOnPositionChangedEvent().addListener([this](auto pos) {
                    TaskManager::getInstance()->addTask([this]() {
                        if (SceneManager::getInstance()->getCurrentScene())
                            SceneManager::getInstance()->getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y + 25.f }); // Title bar size
                    });
                });

                // Size changed event
                getOnSizeChangedEvent().addListener([this](auto size) {
                    TaskManager::getInstance()->addTask([this]() {
                        auto size = getSize();
                        m_fbo->Resize(size.x, size.y);
                        m_imageWidget->setSize(size);

                        // Adjust camera aspect ratio
                        if (SceneManager::getInstance()->getCurrentScene() 
                            && SceneManager::getInstance()->getCurrentScene()->getActiveCamera())                        
                            SceneManager::getInstance()->getCurrentScene()->getActiveCamera()->setAspectRatio(size.x / size.y);                        

                        if (SceneManager::getInstance()->getCurrentScene())
                            SceneManager::getInstance()->getCurrentScene()->setWindowSize({ getSize().x, getSize().y });
                    });
                });

                // Adjust camera aspect ratio
                if (SceneManager::getInstance()->getCurrentScene() && SceneManager::getInstance()->getCurrentScene()->getActiveCamera())
                {
                    SceneManager::getInstance()->getCurrentScene()->getActiveCamera()->setAspectRatio(size.x / size.y);
                }

                // Initialize window pos and size
                if (SceneManager::getInstance()->getCurrentScene())
                {
                    SceneManager::getInstance()->getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y + 25.f }); // Title bar size
                    SceneManager::getInstance()->getCurrentScene()->setWindowSize({ getSize().x, getSize().y });
                }

                m_bInitialized = true;
            }
        }
    }

    void GameScene::update(float dt)
    {
        if (!m_bIsPlaying || m_bIsPausing)
            return;

        // Ensure initialization
        initialize();

        if (!m_bInitialized)
            return;

        //! Update Panel
        Panel::update(dt);

        // Update scene
        SceneManager::getInstance()->update(dt);

        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, {0.f, 0.f, 0.f, 0.f}, true, true);
            SceneManager::getInstance()->render();
            renderContext->EndScene();
        }
    }

    void GameScene::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void GameScene::play()
    {
        if (!m_bIsPlaying)
        {
            SceneManager::getInstance()->setIsEditor(false);

            if (SceneManager::getInstance()->getCurrentScene())
            {
                auto path = SceneManager::getInstance()->getCurrentScene()->getName() + "_tmp";
                auto& selectedObj = Editor::getInstance()->getSelectedObject();
                m_lastObjectId = selectedObj ? selectedObj->getId() : -1;
                Editor::getCanvas()->getEditorScene()->setTargetObject(nullptr);
                Editor::getInstance()->refreshScene();
                SceneManager::getInstance()->saveScene(path);
            }
            m_bIsPlaying = true;
        }
        
        m_bIsPausing = false;

        setFocus();
    }

    void GameScene::pause()
    {
        m_bIsPausing = true;
    }

    void GameScene::stop()
    {
        if (m_bIsPlaying)
        {
            clear();

            SceneManager::getInstance()->setIsEditor(true);

            if (SceneManager::getInstance()->getCurrentScene())
            {
                auto name = SceneManager::getInstance()->getCurrentScene()->getName();
                Editor::getInstance()->refreshScene();
                Editor::getCanvas()->getHierarchy()->clear();
                Editor::getCanvas()->getHierarchy()->initialize();
                auto& scene = Editor::getCurrentScene();
                if (scene)
                {
                    SceneManager::getInstance()->unloadScene(scene);
                    scene = nullptr;
                }

                scene = SceneManager::getInstance()->loadScene(name + "_tmp");
                SceneManager::getInstance()->setCurrentScene(scene);

                Editor::getInstance()->setSelectedObject(m_lastObjectId);
                m_lastObjectId = -1;

                auto fsPath = fs::path(name + "_tmp");
                auto ext = fsPath.extension();
                if (ext.string() != ".scene")
                    fsPath = fsPath.replace_extension(".scene");
                fs::remove(fsPath);
            }

            m_bIsPausing = false;
            m_bIsPlaying = false;
        }
        Editor::getCanvas()->getEditorScene()->setFocus();
    }
}
