#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"
#include "core/ShapeDrawer.h"

#include "utils/GraphicsHelper.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"
#include "utils/RayOBBChecker.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/physic/PhysicBox.h"
#include "components/physic/PhysicSphere.h"
#include "components/physic/PhysicCapsule.h"
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
        m_currentScene = nullptr;
        m_gizmo = nullptr;

        m_grid2D = nullptr;
        m_grid3D = nullptr;

        m_currentCamera = nullptr;

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

                ShapeDrawer::initialize();

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

        // Update render target size
        if (m_bNeedResize)
        {
            auto size = getSize();
            m_fbo->Resize(size.x, size.y);
            m_imageWidget->setSize(size);
            if(Editor::getCurrentScene())
                onCameraChanged(Editor::getCurrentScene()->getActiveCamera());
            m_bNeedResize = false;
        }

        //! If there is no scene, just do nothing
        if (!m_bIsInitialized || !Editor::getCurrentScene() || !Editor::getCurrentScene()->getActiveCamera())
        {
            auto renderContext = RenderContext::InstancePtr();
            if (renderContext && m_fbo)
            {
                renderContext->BeginScene(m_fbo, Vec4(0.2f, 0.2f, 0.2f, 1.f), true, true);
                renderContext->EndScene();
            }
            return;
        }

        // Update active camera
        if (Editor::getCurrentScene() != m_currentScene)
        {
            m_currentScene = Editor::getCurrentScene();
            onCameraChanged(m_currentScene->getActiveCamera());

            m_currentScene->getOnActiveCameraChangedEvent().addListener([this](auto camera) {
                onCameraChanged(camera);
            });
        }

        //! Update Panel
        Panel::update(dt);

        //! Update camera
        updateCameraPosition();

        // If left button release, check selected object
        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        if (isFocused() && touch->isFingerReleased(0))
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

            // Render physic debug
            ShapeDrawer::setViewProjectionMatrix(renderContext->GetRenderViewProjectionMatrix());
            renderPhysicDebug();

            renderContext->EndScene();
        }
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void EditorScene::renderPhysicDebug()
    {
        if (!isOpened())
            return;

        auto target = Editor::getInstance()->getSelectedObject();
        if (target == nullptr)
            return;

        auto transform = target->getTransform();
        auto rotation = transform->getWorldRotation();
        auto position = transform->getWorldPosition();
        auto scale = transform->getWorldScale();

        /* Draw the box collider if any */
        if (auto physicBox = target->getComponent<PhysicBox>(); physicBox)
        {
            auto colliderSize = physicBox->getSize();
            Vec3 halfSize = { colliderSize[0] * scale[0], colliderSize[1] * scale[1], colliderSize[2] * scale[2] };
            auto size = halfSize * 2.f;

            ShapeDrawer::drawLine(position + rotation * Vec3{ -halfSize[0], -halfSize[1], -halfSize[2] }, position + rotation * Vec3{ -halfSize[0], -halfSize[1], +halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ -halfSize[0], halfSize[1], -halfSize[2] }, position + rotation * Vec3{ -halfSize[0], +halfSize[1], +halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ -halfSize[0], -halfSize[1], -halfSize[2] }, position + rotation * Vec3{ -halfSize[0], +halfSize[1], -halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ -halfSize[0], -halfSize[1], +halfSize[2] }, position + rotation * Vec3{ -halfSize[0], +halfSize[1], +halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ +halfSize[0], -halfSize[1], -halfSize[2] }, position + rotation * Vec3{ +halfSize[0], -halfSize[1], +halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ +halfSize[0], halfSize[1], -halfSize[2] }, position + rotation * Vec3{ +halfSize[0], +halfSize[1], +halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ +halfSize[0], -halfSize[1], -halfSize[2] }, position + rotation * Vec3{ +halfSize[0], +halfSize[1], -halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ +halfSize[0], -halfSize[1], +halfSize[2] }, position + rotation * Vec3{ +halfSize[0], +halfSize[1], +halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ -halfSize[0], -halfSize[1], -halfSize[2] }, position + rotation * Vec3{ +halfSize[0], -halfSize[1], -halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ -halfSize[0], +halfSize[1], -halfSize[2] }, position + rotation * Vec3{ +halfSize[0], +halfSize[1], -halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ -halfSize[0], -halfSize[1], +halfSize[2] }, position + rotation * Vec3{ +halfSize[0], -halfSize[1], +halfSize[2] });
            ShapeDrawer::drawLine(position + rotation * Vec3{ -halfSize[0], +halfSize[1], +halfSize[2] }, position + rotation * Vec3{ +halfSize[0], +halfSize[1], +halfSize[2] });
        }

        /* Draw the sphere collider if any */
        if (auto physicSphere = target->getComponent<PhysicSphere>(); physicSphere)
        {           
            float radius = physicSphere->getRadius() * std::max(std::max(std::max(scale[0], scale[1]), scale[2]), 0.0f);

            for (float i = 0; i <= 360.0f; i += 10.0f)
            {
                ShapeDrawer::drawLine(position + rotation * (Vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } *radius), position + rotation * (Vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } *radius));
                ShapeDrawer::drawLine(position + rotation * (Vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } *radius), position + rotation * (Vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } *radius));
                ShapeDrawer::drawLine(position + rotation * (Vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *radius), position + rotation * (Vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *radius));
            }
        }

        /* Draw the capsule collider if any */
        if (auto physicCapsule = target->getComponent<PhysicCapsule>(); physicCapsule)
        {
            float radius = abs(physicCapsule->getRadius() * std::max(std::max(scale[01], scale[2]), 0.f));
            float height = abs(physicCapsule->getHeight() * scale[1]);
            float halfHeight = height / 2;

            Vec3 hVec = { 0.0f, halfHeight, 0.0f };
            for (float i = 0; i < 360.0f; i += 10.0f)
            {
                ShapeDrawer::drawLine(position + rotation * (hVec + Vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *radius), position + rotation * (hVec + Vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *radius));
                ShapeDrawer::drawLine(position + rotation * (-hVec + Vec3{ cos(i * (3.14f / 180.0f)), 0.f, sin(i * (3.14f / 180.0f)) } *radius), position + rotation * (-hVec + Vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), 0.f, sin((i + 10.0f) * (3.14f / 180.0f)) } *radius));

                if (i < 180.f)
                {
                    ShapeDrawer::drawLine(position + rotation * (hVec + Vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } *radius), position + rotation * (hVec + Vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } *radius));
                    ShapeDrawer::drawLine(position + rotation * (hVec + Vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } *radius), position + rotation * (hVec + Vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } *radius));
                }
                else
                {
                    ShapeDrawer::drawLine(position + rotation * (-hVec + Vec3{ cos(i * (3.14f / 180.0f)), sin(i * (3.14f / 180.0f)), 0.f } *radius), position + rotation * (-hVec + Vec3{ cos((i + 10.0f) * (3.14f / 180.0f)), sin((i + 10.0f) * (3.14f / 180.0f)), 0.f } *radius));
                    ShapeDrawer::drawLine(position + rotation * (-hVec + Vec3{ 0.f, sin(i * (3.14f / 180.0f)), cos(i * (3.14f / 180.0f)) } *radius), position + rotation * (-hVec + Vec3{ 0.f, sin((i + 10.0f) * (3.14f / 180.0f)), cos((i + 10.0f) * (3.14f / 180.0f)) } *radius));
                }
            }

            ShapeDrawer::drawLine(position + rotation * (Vec3{ -radius, -halfHeight, 0.f }), position + rotation * (Vec3{ -radius, +halfHeight, 0.f }));
            ShapeDrawer::drawLine(position + rotation * (Vec3{ radius, -halfHeight, 0.f }), position + rotation * (Vec3{ radius, +halfHeight, 0.f }));
            ShapeDrawer::drawLine(position + rotation * (Vec3{ 0.f, -halfHeight, -radius }), position + rotation * (Vec3{ 0.f, +halfHeight, -radius }));
            ShapeDrawer::drawLine(position + rotation * (Vec3{ 0.f, -halfHeight, radius }), position + rotation * (Vec3{ 0.f, +halfHeight, radius }));
        }
    }

    void EditorScene::updateCameraPosition()
    {
        if (!isOpened() ||!isFocused())
            return;

        auto target = Editor::getInstance()->getSelectedObject();
        if (target == nullptr || target->isGUIObject())
            return;

        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        if (touch->isFingerScrolled(0))
        {
            float scrollX, scrollY;
            bool isInverse;
            touch->getFingerScrolledData(0, scrollX, scrollY, isInverse);

            Vec2 offset = { scrollX, scrollY };
            auto mouseOffset = offset * m_cameraDragSpeed;

            auto cameraPosition = m_currentCamera->getPosition();
            auto cameraRotation = m_currentCamera->getRotation();

            const Vec3 rightVec = { 1.f, 0.f, 0.f };
            const Vec3 upVec = { 0.f, 0.f, 1.f };
            cameraPosition += cameraRotation * rightVec * mouseOffset.X();
            cameraPosition -= cameraRotation * upVec * mouseOffset.Y();
            m_currentCamera->setPosition(cameraPosition);

        }
        else if (touch->isFingerPressed(0)) 
        {
            touch->getFingerPosition(0, m_lastMousePosX, m_lastMousePosY);
        }
        else if (touch->isFingerMoved(0))
        {
            auto finger = touch->getFinger(0);
            auto fid = finger->getFingerId();
            if (finger->getFingerId() == 3) // right button
            {
                float touchX, touchY;
                touch->getFingerPosition(0, touchX, touchY);

                Vec2 offset = { touchX - m_lastMousePosX, touchY - m_lastMousePosY };
                m_lastMousePosX = touchX;
                m_lastMousePosY = touchY;

                if (m_bIsFirstTouch)
                {
                    auto cameraRotation = m_currentCamera->getRotation();
                    vmath_quatToEuler(cameraRotation.P(), m_cameraRotationEuler.P());

                    //! remove roll
                    if (m_cameraRotationEuler[2] >= 179.0f || m_cameraRotationEuler[2] <= -179.0f)
                    {
                        m_cameraRotationEuler[0] += m_cameraRotationEuler[2];
                        m_cameraRotationEuler[1] = 180.0f - m_cameraRotationEuler[1];
                        m_cameraRotationEuler[2] = 0.0f;
                    }
                    m_bIsFirstTouch = false;
                }

                auto mouseOffset = offset * m_cameraRotationSpeed;
                m_cameraRotationEuler[1] -= mouseOffset.X();
                m_cameraRotationEuler[0] -= mouseOffset.Y();

                Quat rot;
                vmath_eulerToQuat(m_cameraRotationEuler.P(), rot.P());
                m_currentCamera->setRotation(rot);
            }
        }
        else if(touch->isFingerReleased(0))
        {
            m_bIsFirstTouch = true;
        }
    }

    void EditorScene::onCameraChanged(CameraComponent* camera)
    {
        // Cleanup old config
        if (m_currentCamera != camera)
        {
            if (m_currentCamera)
            {
                auto targetObj = m_currentCamera->getShootTarget();
                if (targetObj)
                {
                    auto grid = targetObj->isGUIObject() ? m_grid2D : m_grid3D;
                    targetObj->getShowcase()->Remove(grid);
                }
            }
            
            // Setup new config
            m_currentCamera = camera;

            // Add grids
            if (m_currentCamera)
            {
                auto targetObj = camera->getShootTarget();
                if (targetObj)
                {
                    targetObj->getShowcase()->Add(targetObj->isGUIObject() ? m_grid2D : m_grid3D);
                }
            }
        }
        
        // If new camera is null, just return
        if (m_currentCamera == nullptr)
        {
            return;
        }

        if (m_gizmo)
            m_gizmo->setCamera(m_currentCamera ? m_currentCamera->getCamera() : nullptr);

        auto size = getSize();
        auto targetObj = camera->getShootTarget();
        if (targetObj)
        {
            if (targetObj->isGUIObject())
            {
                if (m_gizmo) m_gizmo->setMode(gizmo::MODE::WORLD);

                auto canvas = targetObj->getRoot()->getComponent<ige::scene::Canvas>();
                if (canvas)
                {
                    auto canvasSize = canvas->getDesignCanvasSize();
                    auto screenSize = Vec2(size.x, size.y);
                    camera->setOrthoHeight(canvasSize.Y() * 0.5f);
                    m_grid2D->SetScale({ canvasSize.Y() * 0.125f , canvasSize.Y() * 0.125f, 1.f });

                    auto transformToViewport = Mat4::Translate(Vec3(-canvasSize.X() * 0.5f, -canvasSize.Y() * 0.5f, 0.f));
                    canvas->setCanvasToViewportMatrix(transformToViewport);
                }
            }
            else
            {
                if (m_gizmo) m_gizmo->setMode(gizmo::MODE::WORLD);
            }
        }

        m_currentCamera->setAspectRatio(size.x / size.y);
    }
}
