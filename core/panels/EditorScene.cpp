#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"

#include "utils/GraphicsHelper.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "utils/RayOBBChecker.h"
#include "components/RectTransform.h"
using namespace ige::scene;

#include <utils/PyxieHeaders.h>
using namespace pyxie;

namespace ige::creator
{
    EditorScene::EditorScene(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {}
    
    EditorScene::~EditorScene()
    {
        clear();
    }

    void EditorScene::clear()
    {
        m_bIsInitialized = false;

        m_imageWidget = nullptr;
        m_gizmo = nullptr;
        m_targetTransform = nullptr;
        
        m_currentCamera = nullptr;
        m_currentShowcase = nullptr;

        getOnSizeChangedEvent().removeAllListeners();
        removeAllWidgets();

        if (m_guiRect)
        {
            m_guiRect->ClearAll();
            m_guiRect->DecReference();
            m_guiRect = nullptr;
        }

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

        if (m_guiCamera)
        {
            m_guiCamera->DecReference();
            m_guiCamera = nullptr;
        }

        if (m_guiShowcase)
        {
            m_guiShowcase->Clear();
            m_guiShowcase->DecReference();
            m_guiShowcase = nullptr;
        }

        if (m_camera)
        {
            m_camera->DecReference();
            m_camera = nullptr;
        }

        if (m_showcase)
        {
            m_showcase->Clear();
            m_showcase->DecReference();
            m_showcase = nullptr;
        }

        ResourceManager::Instance().DeleteDaemon();
    }

    void EditorScene::setTargetObject(std::shared_ptr<SceneObject> obj)
    {
        if (m_targetObject != obj)
        {
            m_targetObject = obj;
            if (m_gizmo)
            {
                m_gizmo->setTarget(m_targetObject);
            }

            if (m_targetObject)
            {
                m_targetTransform = m_targetObject->getComponent<RectTransform>();
                if (m_targetTransform)
                {
                    if (m_guiRect)
                    {
                        if(m_guiShowcase) m_guiShowcase->Remove(m_guiRect);
                        m_guiRect->ClearAll();
                        m_guiRect->DecReference();
                        m_guiRect = nullptr;
                        ResourceManager::Instance().DeleteDaemon();
                    }
                    m_guiRectSize = m_targetTransform->getSize();
                    m_guiRect = GraphicsHelper::getInstance()->createSprite(m_guiRectSize, "sprite/grid");
                    if(m_guiShowcase) m_guiShowcase->Add(m_guiRect);

                    m_currentCamera = m_guiCamera;
                    m_currentShowcase = m_guiShowcase;
                }
                else
                {
                    m_currentCamera = m_camera;
                    m_currentShowcase = m_showcase;
                }
            }
            else
            {
                m_currentCamera = m_camera;
                m_currentShowcase = m_showcase;
            }
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
                m_camera->SetPosition(Vec3(0.f, 5.f, 20.f));
                m_camera->LockonTarget(false);
                m_camera->SetAspectRate(size.x / size.y);

                m_guiCamera = ResourceCreator::Instance().NewCamera("editor_gui_camera", nullptr);
                m_guiCamera->SetPosition(Vec3(0.f, 0.f, 20.f));
                m_guiCamera->LockonTarget(false);
                m_guiCamera->SetAspectRate(size.x / size.y);
                m_guiCamera->SetOrthographicProjection(true);
                m_guiCamera->SetWidthBase(false);
                m_guiCamera->SetOrthoHeight(6.f);

                if (Editor::getSceneManager()->getCurrentScene())
                {
                    m_showcase = Editor::getSceneManager()->getCurrentScene()->getShowcase();
                    m_showcase->IncReference();

                    auto grid = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "sprite/grid");
                    grid->SetPosition(Vec3(0.f, 0.f, 0.f));
                    grid->SetRotation(Quat::RotationX(PI / 2.f));
                    m_showcase->Add(grid);

                    m_guiShowcase = Editor::getSceneManager()->getCurrentScene()->getGuiShowcase();
                    m_guiShowcase->IncReference();

                    auto grid2 = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "sprite/grid");
                    grid->SetPosition(Vec3(0.f, 0.f, 0.f));
                    m_guiShowcase->Add(grid2);
                }

                m_currentCamera = m_camera;
                m_currentShowcase = m_showcase;
          
                getOnSizeChangedEvent().addListener([this](auto size) {
                    auto currSize = getSize();
                    m_bNeedResize = (currSize.x != size.x || currSize.y != size.y);
                });

                m_gizmo = createWidget<Gizmo>();
                m_gizmo->setCamera(m_camera);
                
                m_bIsInitialized = true;
            }
        }
    }

    bool EditorScene::isResizing()
    {       
        auto cursor = ImGui::GetMouseCursor();

        return
            cursor == ImGuiMouseCursor_ResizeEW ||
            cursor == ImGuiMouseCursor_ResizeNS ||
            cursor == ImGuiMouseCursor_ResizeNWSE ||
            cursor == ImGuiMouseCursor_ResizeNESW ||
            cursor == ImGuiMouseCursor_ResizeAll;
    }

    void EditorScene::update(float dt)
    {
        if (m_bNeedResize)
        {
            auto size = getSize();
            m_fbo->Resize(size.x, size.y);
            m_imageWidget->setSize(size);
            m_currentCamera->SetAspectRate(size.x / size.y);
            m_bNeedResize = false;
        }

        if (m_targetTransform && m_guiRect)
        {
            auto rectSize = m_targetTransform->getSize();
            if (m_guiRectSize != rectSize)
            {
                if (m_guiRect)
                {
                    if(m_guiShowcase) m_guiShowcase->Remove(m_guiRect);
                    m_guiRect->ClearAll();
                    m_guiRect->DecReference();
                    m_guiRect = nullptr;
                    ResourceManager::Instance().DeleteDaemon();
                }
                m_guiRectSize = m_targetTransform->getSize();
                m_guiRect = GraphicsHelper::getInstance()->createSprite(m_guiRectSize, "sprite/grid");

                if(m_guiShowcase) m_guiShowcase->Add(m_guiRect);
            }

            m_guiRect->SetPosition(m_targetTransform->getPosition());
            m_guiRect->SetRotation(m_targetTransform->getRotation());
            m_guiRect->SetScale(m_targetTransform->getScale());
        }

        Panel::update(dt);

        if (!m_bIsInitialized)
        {
            initialize();
            return;
        }

        // Update
        if(m_currentCamera) m_currentCamera->Step(dt);
        if(m_currentShowcase) m_currentShowcase->Update(dt);

        // If left button release, check selected object
        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        if (touch->isFingerReleased(0))
        {
            float touchX, touchY;
            touch->getFingerPosition(0, touchX, touchY);

            float screenW = SystemInfo::Instance().GetGameW();
            float screenH = SystemInfo::Instance().GetGameH();

            auto size = getSize();
            float w = size.x;
            float h = size.y;

            auto pos = getPosition();
            float dx = pos.x;
            float dy = pos.y;

            float x = touchX - dx / 2.f;
            float y = touchY + dy / 2.f;

            x = x + (screenW - (w + dx)) / 2.f;
            y = y - (screenH - (h + dy)) / 2.f;

            RayOBBChecker::setChecking(true);

            Mat4 proj;
            m_currentCamera->GetProjectionMatrix(proj);

            Mat4 viewInv;
            m_currentCamera->GetViewInverseMatrix(viewInv);

            RayOBBChecker::screenPosToWorldRay(x, y, w, h, viewInv, proj);
        }

        // Update scene
        if (Editor::getSceneManager())
            Editor::getSceneManager()->update(dt);

        // All object updated, no more checking
        if (RayOBBChecker::isChecking())
        {
            RayOBBChecker::setChecking(false);
        }

        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, Vec4(0.f, 0.0f, 0.0f, 1.f), true, true);

            if(m_currentCamera) m_currentCamera->Render();
            if(m_currentShowcase) m_currentShowcase->Render();

            //if (Editor::getSceneManager())
            //    Editor::getSceneManager()->render();

            renderContext->EndScene();
        }
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }
}
