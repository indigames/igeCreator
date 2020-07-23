#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"

#include "utils/GraphicsHelper.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
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

        ResourceManager::Instance().DeleteDaemon();
    }

    void EditorScene::setTargetObject(std::shared_ptr<SceneObject> obj)
    {
        if (m_targetObject != obj)
        {
            m_targetObject = obj;
            if (m_gizmo) m_gizmo->setTarget(m_targetObject);

            //if (m_targetObject)
            //{
            //    auto rectTransform = m_targetObject->getComponent<RectTransform>();
            //    if (rectTransform && m_guiRectSize != rectTransform->getSize())
            //    {
            //        if (m_guiRect)
            //        {
            //            Editor::getCurrentScene()->getGuiShowcase()->Remove(m_guiRect);
            //            m_guiRect->ClearAll();
            //            m_guiRect->DecReference();
            //            m_guiRect = nullptr;
            //            ResourceManager::Instance().DeleteDaemon();
            //        }
            //        m_guiRectSize = rectTransform->getSize();
            //        m_guiRect = GraphicsHelper::getInstance()->createSprite(m_guiRectSize, "sprite/grid");
            //        Editor::getCurrentScene()->getGuiShowcase()->Add(m_guiRect);

            //        m_currentCamera = m_guiCamera;
            //    }
            //    else
            //    {
            //        m_currentCamera = m_camera;
            //    }
            //}
        }
    }

    void EditorScene::initialize()
    {
        if (Editor::getCurrentScene() && !m_bIsInitialized)
        {
            auto size = getSize();
            if (size.x > 0 && size.y > 0)
            {
                m_rtTexture = ResourceCreator::Instance().NewTexture("Editor_RTTexture", nullptr, size.x, size.y, GL_RGBA);
                m_fbo = ResourceCreator::Instance().NewRenderTarget(m_rtTexture, true, true);
                m_imageWidget = createWidget<Image>(m_fbo->GetColorTexture()->GetTextureHandle(), size);
                /*
                m_guiCamera = ResourceCreator::Instance().NewCamera("editor_gui_camera", nullptr);
                m_guiCamera->SetPosition(Vec3(0.f, 0.f, 20.f));
                m_guiCamera->LockonTarget(false);
                m_guiCamera->SetAspectRate(size.x / size.y);
                m_guiCamera->SetOrthographicProjection(true);
                m_guiCamera->SetWidthBase(false);
                m_guiCamera->SetOrthoHeight(6.f);*/

                Editor::getCurrentScene()->setActiveCamera("default_camera");
          
                getOnSizeChangedEvent().addListener([this](auto size) {
                    auto currSize = getSize();
                    m_bNeedResize = (currSize.x != size.x || currSize.y != size.y);
                });

                m_gizmo = createWidget<Gizmo>();
                m_gizmo->setCamera(Editor::getCurrentScene()->getActiveCamera()->getCamera());
                
                m_grid2D = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "sprite/grid");
                m_grid2D->SetPosition(Vec3(0.f, 0.f, 0.f));

                m_grid3D = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "sprite/grid");
                m_grid3D->SetPosition(Vec3(0.f, 0.f, 0.f));
                m_grid3D->SetRotation(Quat::RotationX(PI / 2.f));

                m_bIsInitialized = true;
            }
        }

        if (Editor::getCurrentScene() != m_currentScene)
        {
            if (m_currentScene)
            {
                //m_currentScene->getGuiShowcase()->Remove(m_grid2D);
                m_currentScene->getActiveCamera()->getShootTarget()->getShowcase()->Remove(m_grid3D);
            }
            m_currentScene = Editor::getCurrentScene();
            //m_currentScene->getGuiShowcase()->Add(m_grid2D);
            m_currentScene->getActiveCamera()->getShootTarget()->getShowcase()->Add(m_grid3D);
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
        // Ensure initialization
        initialize();

        //! If there is no scene, just do nothing
        if (!Editor::getCurrentScene()) return;

        // Update render target size
        if (m_bNeedResize)
        {
            auto size = getSize();
            m_fbo->Resize(size.x, size.y);
            m_imageWidget->setSize(size);
            Editor::getCurrentScene()->getActiveCamera()->setAspectRatio(size.x / size.y);
            m_bNeedResize = false;
        }
        
        //! Update Panel
        Panel::update(dt);

        //! Update GUI Rectangle
/*        if (m_targetObject)
        {
            auto rectTransform = m_targetObject->getComponent<RectTransform>();
            if (rectTransform)
            {
                auto rectSize = rectTransform->getSize();
                if (!m_guiRect || m_guiRectSize != rectSize)
                {
                    if (m_guiRect)
                    {
                        m_currentScene->getGuiShowcase()->Remove(m_guiRect);
                        m_guiRect->ClearAll();
                        m_guiRect->DecReference();
                        m_guiRect = nullptr;
                        ResourceManager::Instance().DeleteDaemon();
                    }
                    m_guiRectSize = rectTransform->getSize();
                    m_guiRect = GraphicsHelper::getInstance()->createSprite(m_guiRectSize, "sprite/grid");
                    m_currentScene->getGuiShowcase()->Add(m_guiRect);
                }
                m_guiRect->SetPosition(rectTransform->getPosition());
                m_guiRect->SetRotation(rectTransform->getRotation());
                m_guiRect->SetScale(rectTransform->getScale());
            }
        } */

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
            float dy = (pos.y + 25.f); // Add title offset

            float x = touchX - dx / 2.f;
            float y = touchY + dy / 2.f;

            x = x + (screenW - (w + dx)) / 2.f;
            y = y - (screenH - (h + dy)) / 2.f;

            Mat4 proj;
            Editor::getCurrentScene()->getActiveCamera()->getProjectionMatrix(proj);

            Mat4 viewInv;
            Editor::getCurrentScene()->getActiveCamera()->getViewInverseMatrix(viewInv);

            RayOBBChecker::screenPosToWorldRay(x, y, w, h, viewInv, proj);

            RayOBBChecker::setChecking(true);
        }

        // Update scene
        Editor::getSceneManager()->update(dt);

        // All object updated, no more checking
        RayOBBChecker::setChecking(false);        

        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, Vec4(0.2f, 0.2f, 0.2f, 1.f), true, true);
            Editor::getSceneManager()->render();
            renderContext->EndScene();
        }
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }
}
