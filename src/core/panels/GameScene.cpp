#include <imgui.h>

#include "core/panels/GameScene.h"
#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"
#include "core/Canvas.h"

#include <utils/GraphicsHelper.h>
#include <scene/Scene.h>
#include <scene/SceneObject.h>
#include <scene/SceneManager.h>
#include <physic/PhysicManager.h>
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
    }

    void GameScene::clear()
    {   
        m_imageWidget = nullptr;
       
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

        ResourceManager::Instance().DeleteDaemon();
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

                getOnSizeChangedEvent().addListener([this](auto size) {
                    auto currSize = getSize();
                    m_bNeedResize = (currSize.x != size.x || currSize.y != size.y);
                });

                m_bInitialized = true;
            }
        }
    }

    void GameScene::update(float dt)
    {
        if (!m_bIsPlaying)
            return;

        // Ensure initialization
        initialize();

        if (!m_bInitialized)
            return;

        // Update render target size
        if (m_bNeedResize)
        {
            auto size = getSize();
            m_fbo->Resize(size.x, size.y);
            m_imageWidget->setSize(size);

            if (Editor::getSceneManager()->getCurrentScene() \
                && Editor::getSceneManager()->getCurrentScene()->getActiveCamera())
            {
                Editor::getSceneManager()->getCurrentScene()->getActiveCamera()->setAspectRatio(size.x / size.y);
            }                
            m_bNeedResize = false;
        }


        //! Update Panel
        Panel::update(dt);

        // Update Physic
        PhysicManager::getInstance()->onUpdate(dt);

        // Update scene
        Editor::getSceneManager()->update(dt);

        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, {0.f, 0.f, 0.f, 0.f}, true, true);
            Editor::getSceneManager()->render();
            renderContext->EndScene();
        }
    }

    void GameScene::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void GameScene::play()
    {
        m_bIsPlaying = true;
        Editor::getCanvas()->getEditorScene()->close();
        Editor::getCanvas()->getEditorScene()->onCameraChanged(nullptr);

        open();
        setFocus();
    }

    void GameScene::pause()
    {
        m_bIsPlaying = false;
    }

    void GameScene::stop()
    {
        m_bIsPlaying = false;
        close();
        Editor::getCanvas()->getEditorScene()->onCameraChanged(Editor::getSceneManager()->getCurrentScene()->getActiveCamera());
        Editor::getCanvas()->getEditorScene()->open();
        Editor::getCanvas()->getEditorScene()->setFocus();
    }
}
