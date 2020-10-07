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

#include <utils/GraphicsHelper.h>
#include <scene/Scene.h>
#include <scene/SceneObject.h>
#include <scene/SceneManager.h>
#include <physic/PhysicManager.h>
#include <audio/AudioManager.h>
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
            if (ext.string() != ".json")
                fsPath = fsPath.replace_extension(".json");
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

                getOnSizeChangedEvent().addListener([this](auto size) {
                    auto currSize = getSize();
                    m_bNeedResize = (currSize.x != size.x || currSize.y != size.y);
                });

                SceneManager::getInstance()->setIsEditor(false);

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

            if (Editor::getSceneManager()->getCurrentScene()
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

        // Update Audio
        AudioManager::getInstance()->onUpdate(dt);

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

        if (SceneManager::getInstance()->getCurrentScene())
        {
            auto path = SceneManager::getInstance()->getCurrentScene()->getName() + "_tmp";
            auto& selectedObj = Editor::getInstance()->getSelectedObject();
            m_lastObjectId = selectedObj ? selectedObj->getId() : -1;
            Editor::getCanvas()->getEditorScene()->setTargetObject(nullptr);
            SceneManager::getInstance()->saveScene(path);
            Editor::getCanvas()->getEditorScene()->close();
            if (PhysicManager::getInstance()->getWorld())
                PhysicManager::getInstance()->getWorld()->clearForces();
        }

        // open();
        setFocus();
    }

    void GameScene::pause()
    {
        m_bIsPlaying = false;
    }

    void GameScene::stop()
    {
        m_bIsPlaying = false;
        // close();
        clear();

        if (SceneManager::getInstance()->getCurrentScene())
        {
            auto name = SceneManager::getInstance()->getCurrentScene()->getName();
            Editor::getInstance()->setSelectedObject(-1);
            Editor::getCanvas()->getHierarchy()->clear();
            Editor::getCanvas()->getHierarchy()->initialize();
            SceneManager::getInstance()->unloadScene(name);
            SceneManager::getInstance()->loadScene(name + "_tmp");

            SceneManager::getInstance()->setIsEditor(true);
            //Editor::getCanvas()->getEditorScene()->open();
            Editor::getCanvas()->getEditorScene()->setFocus();

            Editor::getInstance()->setSelectedObject(m_lastObjectId);
            m_lastObjectId = -1;

            auto fsPath = fs::path(name + "_tmp");
            auto ext = fsPath.extension();
            if (ext.string() != ".json")
                fsPath = fsPath.replace_extension(".json");
            fs::remove(fsPath);
        }
        else
        {
            SceneManager::getInstance()->setIsEditor(true);
            //Editor::getCanvas()->getEditorScene()->open();
            Editor::getCanvas()->getEditorScene()->setFocus();
        }
    }
}
