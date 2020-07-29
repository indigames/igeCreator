#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"

#include "utils/GraphicsHelper.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"
#include "utils/RayOBBChecker.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
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
        m_currentScene = nullptr;

        getOnSizeChangedEvent().removeAllListeners();
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

    void EditorScene::setTargetObject(std::shared_ptr<SceneObject> obj)
    {
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
                          
                getOnSizeChangedEvent().addListener([this](auto size) {
                    auto currSize = getSize();
                    m_bNeedResize = (currSize.x != size.x || currSize.y != size.y);
                });

                m_gizmo = createWidget<Gizmo>();
                m_grid2D = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "sprite/grid");
                m_grid2D->SetPosition(Vec3(0.f, 0.f, 0.f));

                m_grid3D = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "sprite/grid");
                m_grid3D->SetPosition(Vec3(0.f, 0.f, 0.f));
                m_grid3D->SetRotation(Quat::RotationX(PI / 2.f));

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
        // Ensure initialization
        initialize();

        //! If there is no scene, just do nothing
        if (!Editor::getCurrentScene() || !Editor::getCurrentScene()->getActiveCamera())
            return;
        
        // Update active camera
        if (Editor::getCurrentScene() != m_currentScene)
        {
            if ( m_currentScene)
            {
                if(m_currentScene->getActiveCamera())
                {
                    auto targetObj = m_currentScene->getActiveCamera()->getShootTarget();
                    if (targetObj)
                    {
                        auto grid = targetObj->isGUIObject() ? m_grid2D : m_grid3D;
                        targetObj->getShowcase()->Remove(grid);
                    }
                }
                m_currentScene = nullptr;
            }

            m_currentScene = Editor::getCurrentScene();
            if (m_currentScene->getActiveCamera())
            {
                m_gizmo->setCamera(m_currentScene->getActiveCamera()->getCamera());
                auto targetObj = m_currentScene->getActiveCamera()->getShootTarget();
                if (targetObj)
                {
                    auto grid = targetObj->isGUIObject() ? m_grid2D : m_grid3D;
                    targetObj->getShowcase()->Add(grid);

                    /*if (targetObj->isGUIObject())
                    {
                        auto canvas = targetObj->getRoot()->getComponent<ige::scene::Canvas>();
                        if (canvas)
                        {
                            auto canvasSize = canvas->getDesignCanvasSize();
                            auto scale = m_currentScene->getActiveCamera()->getScreenScale();

                            Vec3 scale3(scale.X(), scale.Y(), 1.0f);
                            Vec3 translate3((SystemInfo::Instance().GetDeviceW() - (canvasSize.X() * scale.X())) * 0.5f,
                                (SystemInfo::Instance().GetDeviceH() - (canvasSize.Y() * scale.Y())) * 0.5f, 0.0f);

                            Mat4 canvasToViewportMatrix;
                            canvasToViewportMatrix.Identity();
                            //vmath_mat4_from_rottrans(Quat().P(), translate3.P(), canvasToViewportMatrix.P());
                            //vmath_mat_appendScale(canvasToViewportMatrix.P(), scale3.P(), 4, 4, canvasToViewportMatrix.P());
                            canvas->setCanvasToViewportMatrix(canvasToViewportMatrix);
                        }
                    }*/
                }
            }

            m_currentScene->getOnActiveCameraChangedEvent().addListener([this](auto camera) {
                if (m_currentScene && m_currentScene->getActiveCamera() && m_currentScene->getActiveCamera()->getShootTarget())
                {
                    auto targetObj = m_currentScene->getActiveCamera()->getShootTarget();
                    if (targetObj)
                    {
                        auto grid = targetObj->isGUIObject() ? m_grid2D : m_grid3D;
                        targetObj->getShowcase()->Remove(grid);
                    }
                }
                if (camera)
                {
                    m_gizmo->setCamera(camera->getCamera());
                    auto targetObj = camera->getShootTarget();
                    if (targetObj)
                    {
                        auto grid = targetObj->isGUIObject() ? m_grid2D : m_grid3D;
                        targetObj->getShowcase()->Add(grid);

                        /*if (targetObj->isGUIObject())
                        {
                            auto canvas = targetObj->getRoot()->getComponent<ige::scene::Canvas>();
                            if (canvas)
                            {
                                auto canvasSize = canvas->getDesignCanvasSize();
                                auto scale = camera->getScreenScale();

                                Vec3 scale3(scale.X(), scale.Y(), 1.0f);
                                Vec3 translate3((SystemInfo::Instance().GetDeviceW() - (canvasSize.X() * scale.X())) * 0.5f,
                                    (SystemInfo::Instance().GetDeviceH() - (canvasSize.Y() * scale.Y())) * 0.5f, 0.0f);

                                Mat4 canvasToViewportMatrix;
                                canvasToViewportMatrix.Identity();
                                //vmath_mat4_from_rottrans(Quat().P(), translate3.P(), canvasToViewportMatrix.P());
                                //vmath_mat_appendScale(canvasToViewportMatrix.P(), scale3.P(), 4, 4, canvasToViewportMatrix.P());
                                canvas->setCanvasToViewportMatrix(canvasToViewportMatrix);
                            }
                        }*/
                    }
                }
            });
        }

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
