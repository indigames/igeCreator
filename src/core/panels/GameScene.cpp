#include "core/panels/GameScene.h"
#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Hierarchy.h"
#include "core/panels/EditorScene.h"
#include "core/panels/Console.h"
#include "core/dialog/SaveFileDialog.h"
#include "core/task/TaskManager.h"
#include "core/filesystem/FileSystem.h"

#include <utils/GraphicsHelper.h>
#include <scene/Scene.h>
#include <scene/SceneObject.h>
#include <scene/SceneManager.h>
#include <event/Event.h>
using namespace ige::scene;

#include <utils/PyxieHeaders.h>
using namespace pyxie;



#include <imgui.h>

namespace ige::creator
{
    extern void startWatcherThread();
    extern void stopWatcherThread();

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
            m_windowSize = Editor::getCurrentScene()->getCanvas() ? Editor::getCurrentScene()->getCanvas()->getTargetCanvasSize() : Vec2(getSize().x, getSize().y);
            if (m_windowSize.X() > 0 && m_windowSize.Y() > 0)
            {
                m_rtTexture = ResourceCreator::Instance().NewTexture("GameScene_RTTexture", nullptr, m_windowSize.X() * 2.f, m_windowSize.Y() * 2.f, GL_RGB, true);
                m_rtTexture->WaitInitialize();
                m_fbo = ResourceCreator::Instance().NewRenderTarget(m_rtTexture, true, true);
                m_fbo->WaitInitialize();

                m_imageWidget = createWidget<Image>(m_fbo->GetColorTexture()->GetTextureHandle(), ImVec2(m_windowSize.X(), m_windowSize.Y()), true);

                // Position changed event
                getOnPositionChangedEvent().addListener([this](auto pos) {
                    TaskManager::getInstance()->addTask([this]() {
                        if (Editor::getCurrentScene())
                            Editor::getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y });
                    });
                });

                // Initialize window pos and size
                if (Editor::getCurrentScene())
                {
                    Editor::getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y });
                    Editor::getCurrentScene()->setWindowSize(m_windowSize);
                    if(Editor::getCurrentScene()->getActiveCamera())
                        Editor::getCurrentScene()->getActiveCamera()->setAspectRatio(m_windowSize.X() / m_windowSize.Y());
                    if (SceneManager::getInstance()->isPlaying()) {
                        Editor::getCurrentScene()->setViewSize({ getSize().x, getSize().y });
                        Editor::getCurrentScene()->setViewPosition({ getScrollPosition().x, getScrollPosition().y });
                    }
                    else {
                        Editor::getCurrentScene()->setViewSize(m_windowSize);
                    }
                }

                m_inputProcessor = std::make_shared<InputProcessor>();

                if (Editor::getCurrentScene()->getCanvas()) {
                    Editor::getCurrentScene()->getCanvas()->getOnTargetSizeChanged().addListener([this](auto size) {
                        TaskManager::getInstance()->addTask([this, size]() {
                            if (!m_bInitialized) return;

                            m_windowSize = size;

                            m_fbo->Resize(size.X() * 2.f, size.Y() * 2.f);
                            m_imageWidget->setSize({ size.X(), size.Y() });

                            if (Editor::getCurrentScene())
                            {
                                Editor::getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y });
                                Editor::getCurrentScene()->setWindowSize(m_windowSize);
                                Editor::getCurrentScene()->getActiveCamera()->setAspectRatio(m_windowSize.X() / m_windowSize.Y());
                                Editor::getCurrentScene()->setViewSize(m_windowSize);
                            }

                            SceneManager::getInstance()->update(0.f);
                        });
                    });
                }
                m_bInitialized = true;
            }
        }
    }

    void GameScene::update(float dt)
    {
        if (!Editor::getCurrentScene() || Editor::getCurrentScene()->isPrefab())
            return;

        // Ensure initialization
        initialize();

        if (!m_bInitialized)
            return;

        // Update windows pos and size
        if (Editor::getCurrentScene()) {
            Editor::getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y });
            Editor::getCurrentScene()->setWindowSize(m_windowSize);
            if (SceneManager::getInstance()->isPlaying())
            {
                Editor::getCurrentScene()->setViewSize({ getSize().x, getSize().y });
                Editor::getCurrentScene()->setViewPosition({ getScrollPosition().x, getScrollPosition().y });
            }
            else
            {
                Editor::getCurrentScene()->setViewSize(m_windowSize);
            }

            if(Editor::getCurrentScene()->getActiveCamera())
                Editor::getCurrentScene()->getActiveCamera()->setAspectRatio(m_windowSize.X() / m_windowSize.Y());
        }

        // Update
        if (isPlaying() && !isPausing()) {
            //! Update Touch & Keyboard, using for UI Object to capture touch before any raycast
            updateKeyboard();
            updateTouch();

            // Update scene
            SceneManager::getInstance()->update(dt);

            delta_time += dt;
            if (delta_time >= 0.02f) {
                while (delta_time > 0.02f) {
                    delta_time -= 0.02f;
                    SceneManager::getInstance()->fixedUpdate(0.02f);
                }
                delta_time = 0;

            }
            // Physic update scene
            SceneManager::getInstance()->physicUpdate(dt);
        }

        // Pre render
        SceneManager::getInstance()->preRender();

        // Render scene
        
        if (m_fbo) {
            SceneManager::getInstance()->render(m_fbo);
        }

        // Update Panel
        Panel::update(dt);

        // Late Update
        if (isPlaying() && !isPausing()) {
            SceneManager::getInstance()->lateUpdate(dt);
        }
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
            stopWatcherThread();

            if (Editor::getCanvas()->getConsole()->isAutoClearOnStart())
                Editor::getCanvas()->getConsole()->clearAllLogs();

            auto path = Editor::getCurrentScene()->getName() + ".scene.tmp";
            SceneManager::getInstance()->saveScene(path);

            m_bIsPlaying = true;
            SceneManager::getInstance()->setIsPlaying(m_bIsPlaying);
            Editor::getInstance()->loadScene(path);

            clear();
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

            if (Editor::getCurrentScene()) {
                if (Editor::getCurrentScene()->getCanvas())
                    Editor::getCurrentScene()->getCanvas()->getOnTargetSizeChanged().removeAllListeners();
                auto name = Editor::getCurrentScene()->getName();
                Editor::getInstance()->loadScene(name + ".scene.tmp");
                auto fsPath = fs::path(name + ".scene.tmp");
                fs::remove(fsPath);
            }
            m_bIsPausing = false;
            m_bIsPlaying = false;
            SceneManager::getInstance()->setIsPlaying(m_bIsPlaying);

            startWatcherThread();
        }
        Editor::getCanvas()->getEditorScene()->setFocus();
    }

    void GameScene::updateKeyboard()
    {
    }

    void GameScene::updateTouch()
    {
        if (!isHovered()) return;

        auto wSize = Vec2(getSize().x, getSize().y);
        auto delta = (wSize - m_windowSize) * 0.5f;

        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        for (int i = 0; i < touch->getFingersCount(); ++i) {
            auto fingerId = touch->getFingerID(i);
            if (touch->isFingerPressed(i))
            {
                float touchX, touchY;
                touch->getFingerPosition(i, touchX, touchY);
                m_inputProcessor->touchDown(fingerId, touchX - delta.X(), touchY + delta.Y());
            }
            if (touch->isFingerMoved(i))
            {
                float touchX, touchY;
                touch->getFingerPosition(i, touchX, touchY);
                m_inputProcessor->touchMove(fingerId, touchX - delta.X(), touchY + delta.Y());
            }
            if (touch->isFingerReleased(i))
            {
                float touchX, touchY;
                touch->getFingerPosition(i, touchX, touchY);
                m_inputProcessor->touchUp(fingerId, touchX - delta.X(), touchY + delta.Y());
            }
        }
    }
}
