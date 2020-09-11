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

        m_targetObject = nullptr;
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

        // Update render target size
        if (m_bNeedResize)
        {
            auto size = getSize();
            m_fbo->Resize(size.x, size.y);
            m_imageWidget->setSize(size);
            onCameraChanged(Editor::getCurrentScene()->getActiveCamera());
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
        m_targetObject = Editor::getInstance()->getSelectedObject();
        if (m_targetObject == nullptr)
            return;

        auto transform = m_targetObject->getTransform();
        auto rotation = transform->getWorldRotation();
        auto position = transform->getWorldPosition();
        auto scale = transform->getWorldScale();

        /* Draw the box collider if any */
        if (auto physicBox = m_targetObject->getComponent<PhysicBox>(); physicBox)
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
        if (auto physicSphere = m_targetObject->getComponent<PhysicSphere>(); physicSphere)
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
        if (auto physicCapsule = m_targetObject->getComponent<PhysicCapsule>(); physicCapsule)
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

    void EditorScene::onCameraChanged(CameraComponent* camera)
    {
        // If new camera is null, just return
        if (!camera)
        {
            m_currentCamera = nullptr;
            if(m_gizmo) m_gizmo->setCamera(nullptr);
            return;
        }

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
            auto targetObj = camera->getShootTarget();
            if (targetObj)
            {
                if (targetObj->isGUIObject()) targetObj->getShowcase()->Add(m_grid2D);
                else targetObj->getShowcase()->Add(m_grid3D);
            }
        }
        
        if(m_gizmo) m_gizmo->setCamera(camera->getCamera());
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
