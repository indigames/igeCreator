#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"
#include "utils/GraphicsHelper.h"

using namespace pyxie;

namespace ige::creator
{
    EditorScene::EditorScene(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        m_editorShowcase = nullptr;
        m_editorEnvironment = nullptr;
        m_editorCamera = nullptr;
        m_rtTexture = nullptr;
        m_fbo = nullptr;
        m_image = nullptr;
    }
    
    EditorScene::~EditorScene()
    {
        clear();
    }

    void EditorScene::clear()
    {
        if (m_editorShowcase)
            m_editorShowcase->Clear();
        m_editorShowcase = nullptr;
        m_editorEnvironment = nullptr;
        m_editorCamera = nullptr;
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
    }

    void EditorScene::initialize()
    {
        m_editorCamera = ResourceCreator::Instance().NewCamera("Editor_Camera", nullptr);
        m_editorCamera->LockonTarget(false);
        m_editorCamera->SetCameraPosition({ 0.f, 0.f, 10.f });
        m_editorCamera->SetOrthographicProjection(true);

        m_editorEnvironment = ResourceCreator::Instance().NewEnvironmentSet("Editor_Environment", nullptr);
        m_editorEnvironment->SetAmbientGroundColor(Vec3(0.5f, 0.5f, 0.5f));
        m_editorEnvironment->SetDirectionalLampColor(0, Vec3(0.5f, 0.5f, 0.5f));

        m_editorShowcase = ResourceCreator::Instance().NewShowcase();
        m_editorShowcase->Add(m_editorEnvironment);

        auto fig = GraphicsHelper::getInstance()->createSprite({ 512, 512 }, "grid");
        m_editorShowcase->Add(fig);
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

        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            m_editorShowcase->Update(dt);

            renderContext->BeginScene(m_fbo, Vec4(0.f, 0.f, 0.f , 1.f), true, true);
            
            m_editorCamera->Render();
            m_editorShowcase->Render();

            renderContext->EndScene();
        }
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }
}
