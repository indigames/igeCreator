#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"

using namespace pyxie;

namespace ige::creator
{
    EditorScene::EditorScene(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        m_rtTexture = nullptr;
        m_fbo = nullptr;
        m_image = nullptr;
        m_sceneManager = nullptr;
    }
    
    EditorScene::~EditorScene()
    {
        clear();
    }

    void EditorScene::clear()
    {
        m_image = nullptr;

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

        m_sceneManager = nullptr;
    }

    void EditorScene::initialize()
    {
        m_sceneManager = std::make_shared<SceneManager>();
        auto scene = m_sceneManager->createEmptyScene();
        m_sceneManager->setCurrentScene("EmptyScene");
        assert(scene == m_sceneManager->getCurrentScene());
    }

    void EditorScene::update(float dt)
    {
        Panel::update(dt);
        
        if (!m_fbo) {
            auto size = getSize();
            if (size.x > 0 && size.y > 0)
            {
                m_rtTexture = ResourceCreator::Instance().NewTexture("Editor_RTTexture", nullptr, size.x, size.y, GL_RGBA);
                m_fbo = ResourceCreator::Instance().NewRenderTarget(m_rtTexture, true, true);
                m_image = createWidget<Image>(m_fbo->GetColorTexture()->GetTextureHandle(), size);
            }            
        }

        // Update
        if (m_sceneManager) m_sceneManager->update(dt);

        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, Vec4(0.f, 0.0f, 0.0f , 1.f), true, true);           
            if (m_sceneManager) m_sceneManager->render();
            renderContext->EndScene();
        }
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }
}
