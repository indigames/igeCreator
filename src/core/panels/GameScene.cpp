#include <imgui.h>

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
#include "event/Event.h"

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
        if (Editor::getCurrentScene())
        {
            auto name = Editor::getCurrentScene()->getName();
            auto fsPath = fs::path(name + ".scene.tmp");
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

        if (m_inputProcessor)
        {
            m_inputProcessor = nullptr;
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
                        if (Editor::getCurrentScene())
                            Editor::getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y + 25.f }); // Title bar size
                    });
                });

                // Size changed event
                getOnSizeChangedEvent().addListener([this](auto size) {
                    TaskManager::getInstance()->addTask([this]() {
                        auto size = getSize();
                        if (m_fbo == nullptr) {
                            m_rtTexture = ResourceCreator::Instance().NewTexture("GameScene_RTTexture", nullptr, size.x, size.y, GL_RGBA);
                            m_fbo = ResourceCreator::Instance().NewRenderTarget(m_rtTexture, true, true);
                            m_imageWidget = createWidget<Image>(m_fbo->GetColorTexture()->GetTextureHandle(), size);
                        }
                        else {
                            m_fbo->Resize(size.x, size.y);
                            m_imageWidget->setSize(size);
                        }
                        // Adjust camera aspect ratio
                        if (Editor::getCurrentScene() 
                            && Editor::getCurrentScene()->getActiveCamera())                        
                            Editor::getCurrentScene()->getActiveCamera()->setAspectRatio(size.x / size.y);                        

                        if (Editor::getCurrentScene())
                            Editor::getCurrentScene()->setWindowSize({ getSize().x, getSize().y });
                    });
                });

                // Adjust camera aspect ratio
                if (Editor::getCurrentScene() && Editor::getCurrentScene()->getActiveCamera())
                {
                    Editor::getCurrentScene()->getActiveCamera()->setAspectRatio(size.x / size.y);
                }

                // Initialize window pos and size
                if (Editor::getCurrentScene())
                {
                    Editor::getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y + 25.f }); // Title bar size
                    Editor::getCurrentScene()->setWindowSize({ getSize().x, getSize().y });
                }

                m_inputProcessor = std::make_shared<InputProcessor>();

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

        //! Update Touch & Keyboard, using for UI Object to capture touch before any raycast
        updateKeyboard();
        updateTouch();

        // Update scene
        SceneManager::getInstance()->update(dt);

        // Physic update scene
        SceneManager::getInstance()->physicUpdate(dt);
        
        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, {0.f, 0.f, 0.f, 0.f}, true, true);
            SceneManager::getInstance()->render();
            renderContext->EndScene();
        }

        // Late Update scene
        SceneManager::getInstance()->lateUpdate(dt);
    }

    void GameScene::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void GameScene::play()
    {
        if (!Editor::getCurrentScene() || Editor::getCurrentScene()->isPrefab())
            return;

        if (!m_bIsPlaying)
        {
            if (Editor::getCanvas()->getConsole()->isAutoClearOnStart())
                Editor::getCanvas()->getConsole()->clearAllLogs();

            SceneManager::getInstance()->setIsEditor(false);

            if (Editor::getCurrentScene())
            {
                auto path = Editor::getCurrentScene()->getName() + ".scene.tmp";
                SceneManager::getInstance()->saveScene(path);
                Editor::getInstance()->loadScene(path);
            }
            m_bIsPlaying = true;

            SceneManager::getInstance()->dispathEvent((int)EventType::RunEditor);
        }
        
        m_bIsPausing = false;

        setFocus();
    }

    void GameScene::pause()
    {
        m_bIsPausing = true;
        SceneManager::getInstance()->dispathEvent((int)EventType::PauseEditor);
    }

    void GameScene::stop()
    {
        if (m_bIsPlaying)
        {
            clear();

            SceneManager::getInstance()->setIsEditor(true);

            if (Editor::getCurrentScene())
            {
                auto name = Editor::getCurrentScene()->getName();
                Editor::getInstance()->loadScene(name + ".scene.tmp");
                auto fsPath = fs::path(name + ".scene.tmp");
                fs::remove(fsPath);
            }
            m_bIsPausing = false;
            m_bIsPlaying = false;

            SceneManager::getInstance()->dispathEvent((int)EventType::StopEditor);
        }
        Editor::getCanvas()->getEditorScene()->setFocus();
    }

    void GameScene::updateKeyboard()
    {
    }

    void GameScene::updateTouch()
    {
        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        auto isFocus = isFocused();
        auto isHover = isHovered();
        if (!isFocus || !isHover) return;

        if (touch->isFingerPressed(0))
        {
            float touchX, touchY;
            touch->getFingerPosition(0, touchX, touchY);
            m_inputProcessor->touchDown(0, touchX, touchY);
        }

        if (touch->isFingerMoved(0)) 
        {
            float touchX, touchY;
            touch->getFingerPosition(0, touchX, touchY);
            m_inputProcessor->touchMove(0, touchX, touchY);
        }

        if (touch->isFingerReleased(0))
        {
            float touchX, touchY;
            touch->getFingerPosition(0, touchX, touchY);
            m_inputProcessor->touchUp(0, touchX, touchY);
            
        }
    }
}
