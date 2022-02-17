#include "core/panels/AnimatorPreview.h"

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/FileHandle.h"

#include "core/widgets/TextField.h"
#include "core/plugin/DragDropPlugin.h"

#include <utils/GraphicsHelper.h>

#include <imgui.h>

namespace ige::creator
{
    AnimatorPreview::AnimatorPreview(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings) {
        close();

        getOnClosedEvent().addListener([this]() {
            clear();
            m_modelPath.clear();
        });
    }

    AnimatorPreview::~AnimatorPreview() {
        clear();

        if (m_camera) {
            m_camera->DecReference();
            m_camera = nullptr;
        }

        if (m_showcase) {
            m_showcase->Clear();
            m_showcase->DecReference();
            m_showcase = nullptr;
        }
    }

    void AnimatorPreview::clear() {
        m_imageWidget = nullptr;
        if (m_figure && m_showcase) {
            m_showcase->Remove(m_figure);
            m_figure->BindAnimator((BaseFigure::AnimatorSlot)0, (Animator*)nullptr);
            m_figure->DecReference();
        }
        m_figure = nullptr;
        m_bDirty = true;
        m_bInitialized = false;
    }

    void AnimatorPreview::initialize() {
        clear();
        auto size = getSize();
        if (size.x <= 0 || size.y <= 0) return;

        if (m_camera == nullptr) {
            m_camera = ResourceCreator::Instance().NewCamera("animator_preview_camera", nullptr);
            m_camera->SetPosition({ 0.f, 1.f, 4.f });
            m_camera->LockonTarget(false);
            m_camera->SetNearPlane(1.f);
            m_camera->SetFarPlane(10000.f);
            m_camera->SetAspectRate(size.x / size.y);
        }
        
        if (m_showcase == nullptr) {
            m_showcase = ResourceCreator::Instance().NewShowcase("animator_preview_showcase");

            auto grid = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, GetEnginePath("sprites/grid"));
            grid->SetPosition(Vec3(0.f, -0.01f, -0.01f));
            grid->SetRotation(Quat::RotationX(PI / 2.f));
            m_showcase->Add(grid);
        }

        if (m_rtTexture == nullptr) {
            m_rtTexture = ResourceCreator::Instance().NewTexture("animator_preview_RTTexture", nullptr, size.x, size.y, GL_RGBA);
            m_rtTexture->WaitInitialize();
        }

        if (m_fbo == nullptr) {
            m_fbo = ResourceCreator::Instance().NewRenderTarget(m_rtTexture, true, true);
            m_fbo->WaitInitialize();
        }
        else {
            m_fbo->Resize(size.x, size.y);
        }

        if (!m_modelPath.empty()) {
            auto fsPath = fs::path(m_modelPath).replace_extension(".pyxf");
            m_figure = ResourceCreator::Instance().NewFigure(fsPath.string().c_str(), Figure::CloneSkeleton);
            if (m_figure) {
                Editor::getCanvas()->getAnimatorEditor()->setFigure(m_figure);
                m_showcase->Add(m_figure);
            }
        }
  
        m_imageWidget = createWidget<Image>(m_fbo->GetColorTexture()->GetTextureHandle(), size, true);
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure)) {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                setModelPath(path);
                m_bDirty = true;
            });
        }

        m_bDirty = false;
        m_bInitialized = true;
    }

    void AnimatorPreview::setModelPath(const std::string& path) {
        if (m_modelPath.compare(path) != 0) {
            m_modelPath = path;
            if (m_figure) {
                m_figure->BindAnimator((BaseFigure::AnimatorSlot)0, (Animator*)nullptr);
            }
            m_bDirty = true;
        }
    }

    void AnimatorPreview::update(float dt)
    {
        if (m_bDirty || !m_bInitialized) {
            initialize();
        }

        if (!m_bInitialized)
            return;

        if (m_figure) {            
            m_figure->Step(dt);
        }

        // Update camera and showcase
        if (m_camera) {
            m_camera->Step(dt);
            m_showcase->Update(dt);
            m_showcase->ZSort(m_camera);           
            m_camera->Render();
        }

        // Render scene
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo) {
            renderContext->BeginScene(m_fbo, Vec4(0.2f, 0.2f, 0.2f, 1.f), true, true);
            m_showcase->Render();
            renderContext->EndScene();
        }

        //! Update Panel
        Panel::update(dt);
    }

    void AnimatorPreview::_drawImpl() {
        Panel::_drawImpl();
    }
}
