#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"

#include "utils/GraphicsHelper.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"

using namespace pyxie;
using namespace ige::scene;

namespace ige::creator
{
    EditorScene::EditorScene(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        m_camera = nullptr;
        m_showcase = nullptr;
        m_rtTexture = nullptr;
        m_fbo = nullptr;
    }
    
    EditorScene::~EditorScene()
    {
        clear();
    }

    void EditorScene::clear()
    {
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
        if (!m_bIsInitialized)
        {
            auto size = getSize();
            if (size.x > 0 && size.y > 0)
            {
                m_rtTexture = ResourceCreator::Instance().NewTexture("Editor_RTTexture", nullptr, size.x, size.y, GL_RGBA);
                m_fbo = ResourceCreator::Instance().NewRenderTarget(m_rtTexture, true, true);
                m_imageWidget = createWidget<Image>(m_fbo->GetColorTexture()->GetTextureHandle(), size);

                m_camera = ResourceCreator::Instance().NewCamera("editor_camera", nullptr);
                m_camera->SetPosition(Vec3(0.f, 5.f, 10.f));
                m_camera->LockonTarget(false);

                m_showcase = Editor::getInstance()->getSceneManager()->getCurrentScene()->getShowcase();

                auto grid = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "grid");
                grid->SetPosition(Vec3(0.f, 0.f, 0.f));
                grid->SetRotation(Quat::RotationX(PI / 2.f));
                m_showcase->Add(grid);

                getOnSizeChangedEvent().addListener([this](auto size) {
                    m_imageWidget->setSize(size);
                    m_camera->SetAspectRate(size.x / size.y);
                });

                setAlign(Panel::E_HAlign::CENTER, Panel::E_VAlign::MIDDLE);

                m_bIsInitialized = true;
            }
        }
    }

    void EditorScene::update(float dt)
    {
        Panel::update(dt);

        if (!m_bIsInitialized)
        {
            initialize();
            return;
        }

        // Update
        m_camera->Step(dt);
        m_showcase->Update(dt);
        if (Editor::getInstance()->getSceneManager())
            Editor::getInstance()->getSceneManager()->update(dt);

        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, Vec4(0.f, 0.0f, 0.0f, 1.f), true, true);

            m_camera->Render();

            if (Editor::getInstance()->getSceneManager())
                Editor::getInstance()->getSceneManager()->render();

            renderContext->EndScene();
        }
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }
}
