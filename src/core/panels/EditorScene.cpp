#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/panels/GameScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/ShapeDrawer.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/FileHandle.h"
#include "core/BulletDebugRender.h"

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
#include "components/audio/AudioSource.h"
#include "components/particle/Particle.h"
using namespace ige::scene;

#include <utils/PyxieHeaders.h>
using namespace pyxie;

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

#include <systems/physic/PhysicManager.h>
#include <systems/particle/ParticleManager.h>
using namespace ige::scene;

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

        m_currCamera = nullptr;

        if (m_2dCamera)
            m_2dCamera->DecReference();
        m_2dCamera = nullptr;

        if (m_3dCamera)
            m_3dCamera->DecReference();
        m_3dCamera = nullptr;

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

                m_grid2D = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "sprite/grid");
                m_grid2D->SetPosition(Vec3(0.f, 0.f, 0.f));

                m_grid3D = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, "sprite/grid");
                m_grid3D->SetPosition(Vec3(0.f, 0.f, 0.f));
                m_grid3D->SetRotation(Quat::RotationX(PI / 2.f));

                m_2dCamera = ResourceCreator::Instance().NewCamera("editor_2d_camera", nullptr);
                m_2dCamera->SetPosition({ 0.f, 0.f, 10.f });
                m_2dCamera->LockonTarget(false);
                m_2dCamera->SetAspectRate(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());
                m_2dCamera->SetOrthographicProjection(true);
                m_2dCamera->SetWidthBase(false);

                m_3dCamera = ResourceCreator::Instance().NewCamera("editor_3d_camera", nullptr);
                m_3dCamera->SetPosition({ 0.f, 3.f, 10.f });
                m_3dCamera->LockonTarget(false);
                m_3dCamera->SetAspectRate(SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH());

                m_currCamera = m_3dCamera;
                
                m_gizmo = createWidget<Gizmo>();
                m_gizmo->setMode(Editor::getInstance()->isLocalGizmo() ? gizmo::MODE::LOCAL : gizmo::MODE::WORLD); 
                m_gizmo->setCamera(m_currCamera);

                // Initialize Shape Drawer
                ShapeDrawer::initialize();

                // Initialize Physic Debugger
                static BulletDebugRender* debugRenderer = new BulletDebugRender();
                PhysicManager::getInstance()->getWorld()->setDebugDrawer(debugRenderer);

                initDragDrop();

                m_bIsInitialized = true;
            }
        }
    }

    void EditorScene::initDragDrop()
    {
        // Scene drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Scene))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (!path.empty()) {
                    Editor::getInstance()->setSelectedObject(-1);

                    auto& scene = Editor::getCurrentScene();
                    if (scene) Editor::getSceneManager()->unloadScene(scene);
                    scene = nullptr;

                    Editor::getCanvas()->getHierarchy()->clear();
                    Editor::getCanvas()->getHierarchy()->initialize();
                    Editor::getSceneManager()->loadScene(path);
                }
            });
        }

        // Figure drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getInstance()->getCurrentScene() && !path.empty()) {
                    const auto& currentObject = Editor::getInstance()->getSelectedObject();
                    Editor::getInstance()->getCurrentScene()->createObject(fs::path(path).stem(), currentObject)->addComponent<FigureComponent>(path);
                }
            });
        }

        // Sprite drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getInstance()->getCurrentScene() && !path.empty()) {
                    const auto& currentObject = Editor::getInstance()->getSelectedObject();
                    Editor::getInstance()->getCurrentScene()->createObject(fs::path(path).stem(), currentObject)->addComponent<SpriteComponent>(path);
                }
            });
        }

        // Audio drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Audio))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getInstance()->getCurrentScene() && !path.empty()) {
                    const auto& currentObject = Editor::getInstance()->getSelectedObject();
                    Editor::getInstance()->getCurrentScene()->createObject(fs::path(path).stem(), currentObject)->addComponent<AudioSource>(path);
                }
            });
        }

        // Prefab drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Prefab))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getInstance()->getCurrentScene() && !path.empty()) {
                    const auto& currentObject = Editor::getInstance()->getSelectedObject();
                    Editor::getInstance()->getCurrentScene()->loadPrefab(currentObject->getId(), path);
                }
            });
        }

        // Particle
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Particle))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getInstance()->getCurrentScene() && !path.empty()) {
                    const auto& currentObject = Editor::getInstance()->getSelectedObject();
                    Editor::getInstance()->getCurrentScene()->createObject(fs::path(path).stem(), currentObject)->addComponent<Particle>(path);
                }
            });
        }
    }

    void EditorScene::set2DMode(bool is2D)
    {
        if (is2D && m_currCamera == m_3dCamera)
        {
            // Switch to 2D camera
            m_currCamera = m_2dCamera;

            if (Editor::getInstance()->getSelectedObject())
            {
                auto canvas = Editor::getInstance()->getSelectedObject()->getCanvas();
                if (canvas)
                {
                    auto canvasSize = canvas->getDesignCanvasSize();
                    m_currCamera->SetOrthoHeight(canvasSize.Y() * 0.5f);
                    m_grid2D->SetScale({ canvasSize.Y() * 0.125f , canvasSize.Y() * 0.125f, 1.f });
                }
            }
            else
            {
                const float canvasHeight = 960.f;
                m_currCamera->SetOrthoHeight(canvasHeight * 0.5f);
                m_grid2D->SetScale({ canvasHeight * 0.125f ,canvasHeight * 0.125f, 1.f });
            }

            if (m_currShowcase)
            {
                m_currShowcase->Remove(m_grid3D);
                m_currShowcase->Add(m_grid2D);
            }
        }
        else if (m_currCamera == m_2dCamera)
        {
            // Switch to 3D camera
            m_currCamera = m_3dCamera;

            if (m_currShowcase)
            {
                m_currShowcase->Remove(m_grid2D);
                m_currShowcase->Add(m_grid3D);
            }
        }

        m_gizmo->setCamera(m_currCamera);
    }

    void EditorScene::setTargetObject(const std::shared_ptr<SceneObject>& obj)
    {
        if (!isOpened() || !m_bIsInitialized)
            return;

        if (m_currShowcase)
        {
            if (m_currCamera == m_2dCamera)
                m_currShowcase->Remove(m_grid2D);
            else
                m_currShowcase->Remove(m_grid3D);
        }

        if (obj == nullptr)
        {
            m_currShowcase = nullptr;
            return;
        }

        m_currShowcase = obj->getScene()->getShowcase();
        if (m_currCamera == m_2dCamera)
            m_currShowcase->Add(m_grid2D);
        else
            m_currShowcase->Add(m_grid3D);

        auto canvas = obj->getCanvas();
        if (canvas)
        {
            auto canvasSize = canvas->getDesignCanvasSize();
            if (m_currCamera == m_2dCamera)
            {
                m_currCamera->SetOrthoHeight(canvasSize.Y() * 0.5f);
                m_grid2D->SetScale({ canvasSize.Y() * 0.125f , canvasSize.Y() * 0.125f, 1.f });
            }

            auto transformToViewport = Mat4::Translate(Vec3(-canvasSize.X() * 0.5f, -canvasSize.Y() * 0.5f, 0.f));
            canvas->setCanvasToViewportMatrix(transformToViewport);
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
        if (Editor::getCanvas()->getGameScene()->isPlaying())
            return;

        // Ensure initialization
        initialize();

        // Update render target size
        if (m_bNeedResize)
        {
            auto size = getSize();
            m_fbo->Resize(size.x, size.y);
            m_imageWidget->setSize(size);
            if(m_currCamera)
                m_currCamera->SetAspectRate(size.x / size.y);
            m_bNeedResize = false;
        }

        //! If there is no scene, just do nothing
        if (!m_bIsInitialized || !Editor::getCurrentScene() || m_currCamera == nullptr || m_currShowcase == nullptr)
        {
            auto renderContext = RenderContext::InstancePtr();
            if (renderContext && m_fbo)
            {
                renderContext->BeginScene(m_fbo, Vec4(0.2f, 0.2f, 0.2f, 1.f), true, true);
                renderContext->EndScene();
            }
            return;
        }

        //! Update Panel
        Panel::update(dt);

        //! Update camera
        updateCameraPosition();

        // Update object selection
        updateObjectSelection();

        // Update keyboard
        updateKeyboard();

        // Update scene
        Editor::getSceneManager()->update(dt);

        // Update Physic Transform infomation
        PhysicManager::getInstance()->preUpdate();

        // Update particle
        ParticleManager::getInstance()->onUpdate(dt);

        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, Vec4(0.2f, 0.2f, 0.2f, 1.f), true, true);

            // Render camera
            m_currCamera->Step(dt);
            m_currCamera->Render();

            Editor::getSceneManager()->render();

            // Render particle
            ParticleManager::getInstance()->setProjectionMatrix(renderContext->GetRenderProjectionMatrix());
            ParticleManager::getInstance()->setCameraMatrix(renderContext->GetRenderViewMatrix());
            ParticleManager::getInstance()->onRender();

            // Render debug context
            ShapeDrawer::setViewProjectionMatrix(renderContext->GetRenderViewProjectionMatrix());

            // Render bounding box
            renderBoundingBox();

            // Render physic debug
            renderPhysicDebug();

            // Render camera frustum
            renderCameraFrustum();

            renderContext->EndScene();
        }
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }

    //! Update keyboard
    void EditorScene::updateKeyboard()
    {
        // Press "F" key focus on target object
        auto keyboard = Editor::getApp()->getInputHandler()->getKeyboard();
        if (keyboard->wasReleased(KeyCode::KEY_F))
        {
            auto target = Editor::getInstance()->getSelectedObject();
            if (target && m_currCamera)
            {
                auto targetPos = target->getTransform()->getWorldPosition();
                m_currCamera->SetPosition(targetPos + m_currCamera->GetCameraRotation() * Vec3(0.f, 0.f, 1.f) * 20.f);
            }
        }
    }

    //! Object selection with touch/mouse
    void EditorScene::updateObjectSelection()
    {
        // If left button release, check selected object
        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        if (isFocused() && isHovered() && touch->isFingerReleased(0) && touch->getFinger(0)->getFingerId() == 0)
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
            m_currCamera->GetProjectionMatrix(proj);

            Mat4 viewInv;
            m_currCamera->GetViewInverseMatrix(viewInv);

            RayOBBChecker::screenPosToWorldRay(x, y, w, h, viewInv, proj);

            bool intersected = false;
            float distance;

            // Sort the object from the camera
            auto& objects = SceneManager::getInstance()->getCurrentScene()->getObjects();
            std::sort(objects.begin(), objects.end(), [this](const auto& elem1, const auto& elem2) {
                auto elem1Pos = elem1->getTransform()->getWorldPosition();
                auto elem2Pos = elem2->getTransform()->getWorldPosition();
                auto camPos = m_currCamera->GetCameraPosition();
                return Vec3::Length(elem1Pos - camPos) < Vec3::Length(elem2Pos - camPos);
            });

            for (const auto& obj : objects)
            {
                if (obj)
                {
                    auto transform = obj->getTransform();
                    auto aabb = transform->getAABB();
                    intersected = RayOBBChecker::checkIntersect(aabb.MinEdge, aabb.MaxEdge, transform->getWorldMatrix(), distance);

                    // Update selected info
                    obj->setSelected(intersected);

                    if (intersected)
                        break;
                }
            }
        }
    }

    void EditorScene::renderBoundingBox()
    {
        auto target = Editor::getInstance()->getSelectedObject();
        if (target == nullptr)
            return;
                
        auto transform = target->getTransform();
        auto aabb = transform->getAABB();
        auto position = transform->getWorldPosition() + aabb.getCenter();

        auto min = aabb.MinEdge;
        auto max = aabb.MaxEdge;

        auto colliderSize = (min.Abs() + max.Abs()) * 0.5f;
        Vec3 halfSize = { colliderSize[0], colliderSize[1], colliderSize[2] };

        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], -halfSize[1], -halfSize[2] }, position + Vec3{ -halfSize[0], -halfSize[1], +halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], +halfSize[1], -halfSize[2] }, position + Vec3{ -halfSize[0], +halfSize[1], +halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], -halfSize[1], -halfSize[2] }, position + Vec3{ -halfSize[0], +halfSize[1], -halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], -halfSize[1], +halfSize[2] }, position + Vec3{ -halfSize[0], +halfSize[1], +halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], -halfSize[1], -halfSize[2] }, position + Vec3{ +halfSize[0], -halfSize[1], +halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], +halfSize[1], -halfSize[2] }, position + Vec3{ +halfSize[0], +halfSize[1], +halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], -halfSize[1], -halfSize[2] }, position + Vec3{ +halfSize[0], +halfSize[1], -halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], -halfSize[1], +halfSize[2] }, position + Vec3{ +halfSize[0], +halfSize[1], +halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], -halfSize[1], -halfSize[2] }, position + Vec3{ +halfSize[0], -halfSize[1], -halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], +halfSize[1], -halfSize[2] }, position + Vec3{ +halfSize[0], +halfSize[1], -halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], -halfSize[1], +halfSize[2] }, position + Vec3{ +halfSize[0], -halfSize[1], +halfSize[2] }, { 1.f, 0.f, 0.f });
        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], +halfSize[1], +halfSize[2] }, position + Vec3{ +halfSize[0], +halfSize[1], +halfSize[2] }, { 1.f, 0.f, 0.f });
    }

    void EditorScene::renderPhysicDebug()
    {
        if (!isOpened())
            return;

        // Render Physic debug
        PhysicManager::getInstance()->getWorld()->debugDrawWorld();
    }

    //! Render camera frustum
    void EditorScene::renderCameraFrustum()
    {
        if (!isOpened())
            return;

        auto target = Editor::getInstance()->getSelectedObject();
        if (target == nullptr)
            return;

        auto camera = target->getComponent<CameraComponent>();
        if (camera == nullptr)
            return;

        auto transform = target->getTransform();
        auto cameraPos = transform->getWorldPosition();
        auto cameraRotation = transform->getWorldRotation();
        auto cameraForward = transform->getWorldForward().Negative();

        Mat4 proj;
        camera->getProjectionMatrix(proj);
        proj = proj.Transpose();

        auto f_near = camera->getNearPlane();
        auto f_far = camera->getFarPlane();

        const auto nLeft = f_near * (proj.P()[8] - 1.0f) / proj.P()[0];
        const auto nRight = f_near * (1.0f + proj.P()[8]) / proj.P()[0];
        const auto nTop = f_near * (1.0f + proj.P()[9]) / proj.P()[5];
        const auto nBottom = f_near * (proj.P()[9] - 1.0f) / proj.P()[5];

        const auto fLeft = f_far * (proj.P()[8] - 1.0f) / proj.P()[0];
        const auto fRight = f_far * (1.0f + proj.P()[8]) / proj.P()[0];
        const auto fTop = f_far * (1.0f + proj.P()[9]) / proj.P()[5];
        const auto fBottom = f_far * (proj.P()[9] - 1.0f) / proj.P()[5];

        auto a = cameraRotation * Vec3{ nLeft, nTop, 0 };
        auto b = cameraRotation * Vec3{ nRight, nTop, 0 };
        auto c = cameraRotation * Vec3{ nLeft, nBottom, 0 };
        auto d = cameraRotation * Vec3{ nRight, nBottom, 0 };
        auto e = cameraRotation * Vec3{ fLeft, fTop, 0 };
        auto f = cameraRotation * Vec3{ fRight, fTop, 0 };
        auto g = cameraRotation * Vec3{ fLeft, fBottom, 0 };
        auto h = cameraRotation * Vec3{ fRight, fBottom, 0 };

        // Convenient lambda to draw a frustum line
        auto draw = [&](const Vec3& startPos, const Vec3& endPos, const float planeDistance)
        {
            auto offset = cameraPos + cameraForward * planeDistance;
            auto start = offset + startPos;
            auto end = offset + endPos;
            ShapeDrawer::drawLine(start, end, {1.f, 1.f, 1.f});
        };

        // Draw near plane
        draw(a, b, f_near);
        draw(b, d, f_near);
        draw(d, c, f_near);
        draw(c, a, f_near);

        // Draw far plane
        draw(e, f, f_far);
        draw(f, h, f_far);
        draw(h, g, f_far);
        draw(g, e, f_far);

        // Draw lines between near and far planes
        draw(a + cameraForward * f_near, e + cameraForward * f_far, 0);
        draw(b + cameraForward * f_near, f + cameraForward * f_far, 0);
        draw(c + cameraForward * f_near, g + cameraForward * f_far, 0);
        draw(d + cameraForward * f_near, h + cameraForward * f_far, 0);
    }

    void EditorScene::updateCameraPosition()
    {
        if (!isOpened() ||!isFocused() || m_currCamera == nullptr || m_currCamera == m_2dCamera)
            return;

        auto target = Editor::getInstance()->getSelectedObject();
        if (target == nullptr)
            return;

        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        if (touch->isFingerScrolled(0))
        {
            float scrollX, scrollY;
            bool isInverse;
            touch->getFingerScrolledData(0, scrollX, scrollY, isInverse);

            Vec2 offset = { scrollX, scrollY };
            auto mouseOffset = offset * m_cameraDragSpeed;

            auto cameraPosition = m_currCamera->GetPosition();
            auto cameraRotation = m_currCamera->GetRotation();

            const Vec3 rightVec = { 1.f, 0.f, 0.f };
            const Vec3 upVec = { 0.f, 0.f, 1.f };
            cameraPosition += cameraRotation * rightVec * mouseOffset.X();
            cameraPosition -= cameraRotation * upVec * mouseOffset.Y();
            m_currCamera->SetPosition(cameraPosition);

        }

        if (touch->isFingerMoved(0))
        {
            auto finger = touch->getFinger(0);

            if (m_bIsFirstTouch)
            {
                touch->getFingerPosition(0, m_lastMousePosX, m_lastMousePosY);
                auto cameraRotation = m_currCamera->GetRotation();
                vmath_quatToEuler(cameraRotation.P(), m_cameraRotationEuler.P());
                m_bIsFirstTouch = false;
            }

            float touchX, touchY;
            touch->getFingerPosition(0, touchX, touchY);

            Vec2 offset = { touchX - m_lastMousePosX, touchY - m_lastMousePosY };
            m_lastMousePosX = touchX;
            m_lastMousePosY = touchY;

            if (finger->getFingerId() == 1) // middle button
            {
                auto pos = m_currCamera->GetPosition();
                offset *= std::abs(pos.Z() / (m_currCamera->GetFarPlane() * 0.5f));
                m_currCamera->SetPosition(Vec3(pos.X() - offset.X(), pos.Y() - offset.Y(), pos.Z()));
            }
            else if (finger->getFingerId() == 3) // right button
            {
                auto mouseOffset = offset * m_cameraRotationSpeed;
                m_cameraRotationEuler[1] += mouseOffset.X();
                m_cameraRotationEuler[0] += mouseOffset.Y();

                Quat rot;
                vmath_eulerToQuat(m_cameraRotationEuler.P(), rot.P());
                m_currCamera->SetRotation(rot);
            }
        }

        if(touch->isFingerReleased(0))
        {
            m_bIsFirstTouch = true;
        }
    }
}
