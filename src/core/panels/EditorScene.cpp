#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/panels/GameScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/FileHandle.h"
#include "core/task/TaskManager.h"
#include "core/shortcut/ShortcutController.h"

#include "utils/GraphicsHelper.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "scene/SceneManager.h"
#include "utils/RayOBBChecker.h"
#include "utils/ShapeDrawer.h"
#include "components/gui/RectTransform.h"
#include "components/gui/Canvas.h"
#include "components/physic/PhysicBox.h"
#include "components/physic/PhysicSphere.h"
#include "components/physic/PhysicCapsule.h"
#include "components/audio/AudioSource.h"
#include "components/particle/Particle.h"
#include "components/navigation/DynamicNavMesh.h"
#include "components/navigation/OffMeshLink.h"

#include "components/tween/Tween.h"
#include "components/tween/Tweener.h"


using namespace ige::scene;

#include <utils/PyxieHeaders.h>
using namespace pyxie;

#include "utils/filesystem.h"
//#include <iostream>
namespace fs = ghc::filesystem;

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
        resetShowcase();

        m_bIsInitialized = false;

        if (m_imageWidget) m_imageWidget.reset();
        m_imageWidget = nullptr;

        m_currentScene = nullptr;
        if (m_gizmo) m_gizmo.reset();
        m_gizmo = nullptr;

        if(m_grid2D) m_grid2D->DecReference();
        m_grid2D = nullptr;

        if(m_grid3D) m_grid3D->DecReference();
        m_grid3D = nullptr;

        m_currCamera = nullptr;
        if (m_2dCamera) m_2dCamera->DecReference();
        m_2dCamera = nullptr;

        if (m_3dCamera) m_3dCamera->DecReference();
        m_3dCamera = nullptr;

        getOnSizeChangedEvent().removeAllListeners();
        removeAllWidgets();

        if (m_rtTexture) m_rtTexture->DecReference();
        m_rtTexture = nullptr;        

        if (m_fbo) m_fbo->DecReference();
        m_fbo = nullptr;        
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

                // Position changed event
                getOnPositionChangedEvent().addListener([this](auto pos) {
                    TaskManager::getInstance()->addTask([this]() {
                        if (Editor::getCurrentScene())
                            Editor::getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y + 25.f }); // Title bar size
                        });
                    });

                // Size changed event
                getOnSizeChangedEvent().addListener([this](auto size) {
                    TaskManager::getInstance()->addTask([this]() {
                        auto size = getSize();
                        m_fbo->Resize(size.x, size.y);
                        m_imageWidget->setSize(size);

                        // Update camera aspect rate
                        if (m_currCamera)
                            m_currCamera->SetAspectRate(size.x / size.y);

                        // Update window pos and size
                        if (Editor::getCurrentScene())
                            Editor::getCurrentScene()->setWindowSize({ getSize().x, getSize().y });
                        });
                    });

                m_grid2D = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, GetEnginePath("sprites/grid"));
                m_grid2D->SetPosition(Vec3(0.f, 0.f, 0.f));

                m_grid3D = GraphicsHelper::getInstance()->createGridMesh({ 10000, 10000 }, GetEnginePath("sprites/grid"));
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

                m_currCamera = Editor::getInstance()->is3DCamera() ? m_3dCamera : m_2dCamera;

                m_gizmo = createWidget<Gizmo>();
                m_gizmo->setMode(Editor::getInstance()->isLocalGizmo() ? gizmo::MODE::LOCAL : gizmo::MODE::WORLD);
                m_gizmo->setCamera(m_currCamera);

                initDragDrop();

                // Initialize window pos and size
                if (Editor::getCurrentScene())
                {
                    Editor::getCurrentScene()->setWindowPosition({ getPosition().x, getPosition().y + 25.f }); // Title bar size
                    Editor::getCurrentScene()->setWindowSize({ getSize().x, getSize().y });
                }

                //viet.nv : create dummy object to focus
                m_focusPosition = Vec3(0, 0, 0);
                m_resetFocus = false;
                m_viewSize = k_defaultViewSize;
                updateFocusPoint(true, true);

                m_currentCanvasHeight = SystemInfo::Instance().GetGameH();
                m_canvasRatio = SystemInfo::Instance().GetGameW() / SystemInfo::Instance().GetGameH();

                m_HandleCameraTouchId = -1;
                m_bIsInitialized = true;

                // Target the root node at launch
                if (Editor::getCurrentScene() && Editor::getCurrentScene()->getTargets().size() > 0)
                    setTargetObject(Editor::getCurrentScene()->getTargets().at(0));
            }
        }
    }

    //! Register Shortcut functions
    void EditorScene::registerShortcut() {
        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::EDIT_SCENE_OBJECT_SELECTED, CALLBACK_0(EditorScene::lookSelectedObject, this));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::EDIT_SCENE_OBJECT_SELECTED, KeyCode::KEY_F, false, false, false);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::DELETE_SCENE_OBJECT_SELECTED, CALLBACK_0(Editor::deleteObject, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::DELETE_SCENE_OBJECT_SELECTED, KeyCode::KEY_DEL, false, false, false);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::COPY_SCENE_OBJECT_SELECTED, CALLBACK_0(Editor::copyObject, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::COPY_SCENE_OBJECT_SELECTED, KeyCode::KEY_C, false, true, false);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::PASTE_SCENE_OBJECT_SELECTED, CALLBACK_0(Editor::pasteObject, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::PASTE_SCENE_OBJECT_SELECTED, KeyCode::KEY_V, false, true, false);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::DUPLICATE_SCENE_OBJECT_SELECTED, CALLBACK_0(Editor::cloneObject, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::DUPLICATE_SCENE_OBJECT_SELECTED, KeyCode::KEY_D, false, true, false);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::FILE_NEW_PROJECT_SELECTED, CALLBACK_0(Editor::createProject, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::FILE_NEW_PROJECT_SELECTED, KeyCode::KEY_N, false, true, true);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::FILE_OPEN_PROJECT_SELECTED, CALLBACK_0(Editor::openProject, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::FILE_OPEN_PROJECT_SELECTED, KeyCode::KEY_O, false, true, true);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::FILE_NEW_SCENE_SELECTED, CALLBACK_0(Editor::createScene, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::FILE_NEW_SCENE_SELECTED, KeyCode::KEY_N, true, true, false);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::FILE_SAVE_SCENE_SELECTED, CALLBACK_0(Editor::saveScene, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::FILE_SAVE_SCENE_SELECTED, KeyCode::KEY_S, false, true, false);

        ASSIGN_COMMAND_TO_DICT(ShortcutDictionary::FILE_SAVE_SCENE_AS_SELECTED, CALLBACK_0(Editor::saveSceneAs, Editor::getInstance().get()));
        ASSIGN_KEY_TO_COMMAND(ShortcutDictionary::FILE_SAVE_SCENE_AS_SELECTED, KeyCode::KEY_S, false, true, true);
    }

    void EditorScene::unregisterShortcut() {
        REMOVE_COMMAND(ShortcutDictionary::EDIT_SCENE_OBJECT_SELECTED);
        REMOVE_COMMAND(ShortcutDictionary::DELETE_SCENE_OBJECT_SELECTED);
        REMOVE_COMMAND(ShortcutDictionary::COPY_SCENE_OBJECT_SELECTED);
        REMOVE_COMMAND(ShortcutDictionary::PASTE_SCENE_OBJECT_SELECTED);
        REMOVE_COMMAND(ShortcutDictionary::FILE_NEW_PROJECT_SELECTED);
        REMOVE_COMMAND(ShortcutDictionary::FILE_OPEN_PROJECT_SELECTED);
        REMOVE_COMMAND(ShortcutDictionary::FILE_NEW_SCENE_SELECTED);
        REMOVE_COMMAND(ShortcutDictionary::FILE_SAVE_SCENE_SELECTED);
        REMOVE_COMMAND(ShortcutDictionary::FILE_SAVE_SCENE_SELECTED);
    }

    void EditorScene::initDragDrop()
    {
        // Scene drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Scene))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (!path.empty()) {
                    auto& scene = Editor::getCurrentScene();
                    if (scene) SceneManager::getInstance()->unloadScene(scene);
                    scene = nullptr;

                    Editor::getCanvas()->getHierarchy()->clear();
                    Editor::getCanvas()->getHierarchy()->initialize();
                    SceneManager::getInstance()->loadScene(path);
                }
            });
        }

        // Figure drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Figure))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getCurrentScene() && !path.empty()) {
                    auto targets = Editor::getCurrentScene()->getTargets();
                    const auto& currentObject = (!targets.empty() && targets[0] != nullptr) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                    Editor::getCurrentScene()->createObject(fs::path(path).stem(), currentObject)->addComponent<FigureComponent>(path);
                }
            });
        }

        // Sprite drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getCurrentScene() && !path.empty()) {
                    auto targets = Editor::getCurrentScene()->getTargets();
                    const auto& currentObject = (!targets.empty() && targets[0] != nullptr) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                    Editor::getCurrentScene()->createObject(fs::path(path).stem(), currentObject)->addComponent<SpriteComponent>(path);
                }
            });
        }

        // Audio drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Audio))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getCurrentScene() && !path.empty()) {
                    auto targets = Editor::getCurrentScene()->getTargets();
                    const auto& currentObject = (!targets.empty() && targets[0] != nullptr) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                    Editor::getCurrentScene()->createObject(fs::path(path).stem(), currentObject)->addComponent<AudioSource>(path);
                }
            });
        }

        // Prefab drag/drop
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Prefab))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getCurrentScene() && !path.empty()) {
                    auto targets = Editor::getCurrentScene()->getTargets();
                    const auto& currentObject = (!targets.empty() && targets[0] != nullptr) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                    Editor::getCurrentScene()->loadPrefab(currentObject->getId(), path);
                }
            });
        }

        // Particle
        for (const auto& type : GetFileExtensionSuported(E_FileExts::Particle))
        {
            m_imageWidget->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto path) {
                if (Editor::getCurrentScene() && !path.empty()) {
                    auto targets = Editor::getCurrentScene()->getTargets();
                    const auto& currentObject = (!targets.empty() && targets[0] != nullptr) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
                    Editor::getCurrentScene()->createObject(fs::path(path).stem(), currentObject)->addComponent<Particle>(path);
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

            auto targets = Editor::getCurrentScene()->getTargets();
            const auto& currentObject = (!targets.empty() && targets[0] != nullptr) ? Editor::getCurrentScene()->findObjectById(targets[0]->getId()) : nullptr;
            if (currentObject)
            {
                auto canvas = currentObject->getCanvas();
                if (canvas)
                {
                    auto canvasSize = canvas->getDesignCanvasSize();
                    m_currCamera->SetOrthoHeight(canvasSize.Y() * 0.5f);
                    m_grid2D->SetScale({ canvasSize.Y() * 0.125f , canvasSize.Y() * 0.125f, 1.f });

                    /*auto transformToViewport = Mat4::Translate(Vec3(-canvasSize.X() * 0.5f, -canvasSize.Y() * 0.5f, 0.f));
                    canvas->setCanvasToViewportMatrix(transformToViewport);*/
                    m_currentCanvasHeight = canvasSize.Y();
                    m_canvasRatio = canvasSize.X() / canvasSize.Y();
                }
            }
            else
            {
                m_currentCanvasHeight = SystemInfo::Instance().GetGameH();
                m_currCamera->SetOrthoHeight(m_currentCanvasHeight * 0.5f);
                m_grid2D->SetScale({ m_currentCanvasHeight * 0.125f ,m_currentCanvasHeight * 0.125f, 1.f });
            }

            if (m_currShowcase)
            {
                m_currShowcase->Remove(m_grid3D);
                m_currShowcase->Add(m_grid2D);
            }
        }
        else if (!is2D && m_currCamera == m_2dCamera)
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
    
    bool EditorScene::is2DMode() const {
        return m_currCamera ? m_currCamera == m_2dCamera : false;
    }


    void EditorScene::setTargetObject(SceneObject* obj)
    {
        if (!obj || !isOpened() || !m_bIsInitialized)
            return;

        if (obj->getScene()->getShowcase() == nullptr) return;
        if (!SceneManager::getInstance()->isEditor()) return;

        auto lastShowcase = m_currShowcase;
        m_currShowcase = m_currCamera == m_2dCamera ? obj->getScene()->getUIShowcase() : obj->getScene()->getShowcase();
        
        if (lastShowcase != m_currShowcase)
        {
            if (m_currCamera == m_2dCamera)
            {
                if(lastShowcase)
                    lastShowcase->Remove(m_grid2D);
                m_currShowcase->Add(m_grid2D);
            }
            else
            {
                if(lastShowcase)
                    lastShowcase->Remove(m_grid3D);
                m_currShowcase->Add(m_grid3D);
            }
        }
    }

    void EditorScene::resetShowcase() 
    {
        if (m_currShowcase)
        {
            if (m_currCamera == m_2dCamera)
                m_currShowcase->Remove(m_grid2D);
            else if (m_currCamera == m_3dCamera)
                m_currShowcase->Remove(m_grid3D);
        }
        m_currShowcase = nullptr;
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

        // Update keyboard
        updateKeyboard();

        // Update touch
        updateTouch();

        // Update scene
        SceneManager::getInstance()->update(dt);

        // Render
        auto renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, Vec4(0.2f, 0.2f, 0.2f, 1.f), true, true);

            // Render camera
            m_currCamera->Step(dt);
            m_currCamera->Render();

            // Render scene
            Editor::getCurrentScene()->getShowcase()->ZSort(m_currCamera);
            SceneManager::getInstance()->render();

            // Render bounding box
            renderBoundingBoxes();

            // Render camera frustum
            renderCameraFrustum();

            // Render object select rect
            renderCameraSelect();

            renderContext->EndScene();
        }

        //! Update Panel
        Panel::update(dt);
    }

    void EditorScene::_drawImpl()
    {
        Panel::_drawImpl();
    }

    //! Refresh Scene
    void EditorScene::refresh() {
        resetShowcase();
    }

    //! Update keyboard
    void EditorScene::updateKeyboard()
    {
        auto keyboard = Editor::getApp()->getInputHandler()->getKeyboard();

        m_fnKeyPressed = 0;
        if (keyboard->isKeyDown(KeyCode::KEY_LALT) || keyboard->isKeyDown(KeyCode::KEY_RALT) || keyboard->isKeyHold(KeyCode::KEY_LALT) || keyboard->isKeyHold(KeyCode::KEY_RALT)) {
            m_fnKeyPressed |= SYSTEM_KEYCODE_ALT_MASK;
        }
        if (keyboard->isKeyDown(KeyCode::KEY_LCTRL) || keyboard->isKeyDown(KeyCode::KEY_RCTRL) || keyboard->isKeyHold(KeyCode::KEY_LCTRL) || keyboard->isKeyHold(KeyCode::KEY_RCTRL)) {
            m_fnKeyPressed |= SYSTEM_KEYCODE_CTRL_MASK;
        }
        if (keyboard->isKeyDown(KeyCode::KEY_LSHIFT) || keyboard->isKeyDown(KeyCode::KEY_RSHIFT) || keyboard->isKeyHold(KeyCode::KEY_LSHIFT) || keyboard->isKeyHold(KeyCode::KEY_RSHIFT)) {
            m_fnKeyPressed |= SYSTEM_KEYCODE_SHIFT_MASK;
        }
    }

    void EditorScene::updateTouch() 
    {
        if (!isOpened() || m_gizmo->isUsing() || m_currCamera == nullptr)
            return;

        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        auto isFocus = isFocused();
        auto isHover = isHovered();
        if (touch->isFingerScrolled(0) && isHover)
        {
            auto scroll = Vec2();
            float targetSize;
            bool isInverse;
            touch->getFingerScrolledData(0, scroll[0], scroll[1], isInverse);
            handleCameraScroll(scroll);
        }

        if (touch->isFingerMoved(0))
        {
            auto finger = touch->getFinger(0);
            if (m_HandleCameraTouchId == -1 && (!m_bIsDragging && isHover) ) {
                m_HandleCameraTouchId = finger->getFingerId();
                updateViewTool(finger->getFingerId());
                m_touchDelay = 0;
            }
            else if (finger->getFingerId() == m_HandleCameraTouchId) {
                if(m_touchDelay < 0.02f)
                    m_touchDelay += Time::Instance().GetElapsedTime();
                else 
                {
                    m_bIsDraggingCam = true;
                    float touchX, touchY;
                    touch->getFingerPosition(0, touchX, touchY);
                    
                    updateCameraPosition(touchX, touchY);
                }
            }
            m_bIsDragging = true;
        }

        if (touch->isFingerReleased(0))
        {
            auto finger = touch->getFinger(0);
            if (finger->getFingerId() == m_HandleCameraTouchId || m_HandleCameraTouchId == -1)
            {
                m_lastMousePosX = m_lastMousePosY = -1.f;
                m_firstMousePosX = m_firstMousePosY = -1.f;
                m_bIsDragging = false;
            }

            if (!m_bIsDraggingCam) {
                //Perform Click
                if (isFocus) updateObjectSelection();
                if (finger->getFingerId() == m_HandleCameraTouchId || m_HandleCameraTouchId == -1) {
                    m_HandleCameraTouchId = -1;
                    m_viewTool = ViewTool::None;
                    m_touchDelay = 0;
                }
            }
            else {
                if (finger->getFingerId() == m_HandleCameraTouchId || m_HandleCameraTouchId == -1) {
                    m_HandleCameraTouchId = -1;
                    m_bIsFirstTouch = true;
                    
                    updateFocusPoint(false, m_resetFocus);
                    m_resetFocus = false;
                    
                    m_viewTool = ViewTool::None;
                    m_touchDelay = 0;
                    m_bIsDraggingCam = false;
                }
            }
        }
    }

    void EditorScene::updateViewTool(int touchID) 
    {
        if (m_bIsDraggingCam) return;
        switch (touchID) {
        case 0 : // Left 
            if (Editor::getInstance()->is3DCamera()) {
                //! Important => because this is bit shift, so we can compile multi function key
                //! in this case, only 1 key will be access, so Shift < Ctrl < Alt
                //! Incase nothing press, active Select ViewTool
                m_viewTool = ViewTool::MultiSelectArea;
                if (m_fnKeyPressed & SYSTEM_KEYCODE_ALT_MASK)
                {
                    m_viewTool = ViewTool::Orbit;
                }
            }
            else {
                //! In 2D mode, rotate camera will be locked, only Select Obj & pan available
                m_viewTool = ViewTool::MultiSelectArea;
                if (m_fnKeyPressed & SYSTEM_KEYCODE_ALT_MASK)
                {
                    m_viewTool = ViewTool::Pan;
                }
            }
            break;
        case 1: // Middle
            m_viewTool = ViewTool::Pan;
            break;
        case 2: // Right
            if (Editor::getInstance()->is3DCamera()) {
                m_viewTool = ViewTool::FPS;
                if (m_fnKeyPressed & SYSTEM_KEYCODE_ALT_MASK)
                {
                    m_viewTool = ViewTool::Zoom;
                }
            }
            else {
                m_viewTool = ViewTool::Pan;
                if (m_fnKeyPressed & SYSTEM_KEYCODE_ALT_MASK)
                {
                    m_viewTool = ViewTool::Zoom;
                }
            }
            break;
        default:
            break;
        }
    }

    //! Object selection with touch/mouse
    void EditorScene::updateObjectSelection()
    {
        // If left button release, check selected object
        auto touch = Editor::getApp()->getInputHandler()->getTouchDevice();
        auto keyModifier = Editor::getApp()->getInputHandler()->getKeyboard()->getKeyModifier();
        if (isFocused() && isHovered() && touch->isFingerReleased(0) && touch->getFinger(0)->getFingerId() == 0)
        {
            float touchX, touchY;
            touch->getFingerPosition(0, touchX, touchY);
            auto hit = Editor::getCurrentScene()->raycast(Vec2(touchX, touchY), m_currCamera, 10000.f);
            if (hit.first)
            {
                if (keyModifier & (int)KeyModifier::KEY_MOD_CTRL || keyModifier & (int)KeyModifier::KEY_MOD_SHIFT)
                {
                    if (hit.first->isSelected())
                        Editor::getCurrentScene()->removeTarget(hit.first);
                    else
                        Editor::getCurrentScene()->addTarget(hit.first, false);
                }
                else
                {
                    Editor::getCurrentScene()->addTarget(hit.first, true);
                }
            }
        }
    }

    void EditorScene::renderBoundingBoxes()
    {
        auto targets = Editor::getCurrentScene()->getTargets();
        for (const auto& target : targets)
            renderBoundingBox(target);
    }

    void EditorScene::renderBoundingBox(SceneObject* target)
    {        
        if (target == nullptr)
            return;
        
        auto transform = target->getTransform();
        if (transform == nullptr) return;

        if (!target->isGUIObject()) 
        {
            const auto& aabb = target->getWorldAABB();
            if (aabb.getVolume() <= 0) return;
            auto position = aabb.getCenter();
            Vec3 halfSize = aabb.getExtent() * 0.5f;

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
        else 
        {
            auto canvas = target->getComponent<ige::scene::Canvas>();
            if (canvas)
            {
                const auto& designSize = canvas->getDesignCanvasSize();
                auto position = transform->getPosition();
                Vec2 halfSize = designSize * 0.5f;
                ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], -halfSize[1], 0 }, position + Vec3{ -halfSize[0], +halfSize[1], 0 }, { 1.f, 0.f, 1.f });
                ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], +halfSize[1], 0 }, position + Vec3{ +halfSize[0], +halfSize[1], 0 }, { 1.f, 0.f, 1.f });
                ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], +halfSize[1], 0 }, position + Vec3{ +halfSize[0], -halfSize[1], 0 }, { 1.f, 0.f, 1.f });
                ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], -halfSize[1], 0 }, position + Vec3{ -halfSize[0], -halfSize[1], 0 }, { 1.f, 0.f, 1.f });
            }
            else 
            {
                //! Draw Canvas Parent
                auto canvasParent = target->getCanvas();
                if (canvasParent)
                {
                    const auto& designSize = canvasParent->getDesignCanvasSize();
                    auto canvasTransform = canvasParent->getOwner()->getTransform();
                    if (canvasTransform) {
                        auto& position = canvasTransform->getPosition();
                        Vec2 halfSize = designSize * 0.5f;
                        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], -halfSize[1], 0 }, position + Vec3{ -halfSize[0], +halfSize[1], 0 }, { 1.f, 0.f, 1.f });
                        ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], +halfSize[1], 0 }, position + Vec3{ +halfSize[0], +halfSize[1], 0 }, { 1.f, 0.f, 1.f });
                        ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], +halfSize[1], 0 }, position + Vec3{ +halfSize[0], -halfSize[1], 0 }, { 1.f, 0.f, 1.f });
                        ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], -halfSize[1], 0 }, position + Vec3{ -halfSize[0], -halfSize[1], 0 }, { 1.f, 0.f, 1.f });
                    }
                }

                //! Draw RectTransform
                auto rectTransform = target->getComponent<RectTransform>();
                if (rectTransform)
                {
                    const auto& aabb = target->getWorldAABB();
                    if (aabb.getVolume() <= 0) return;
                    auto position = aabb.getCenter();
                    Vec3 halfSize = aabb.getExtent() * 0.5f;

                    ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], -halfSize[1], 0 }, position + Vec3{ -halfSize[0], +halfSize[1], 0 }, { 1.f, 0.f, 0.f });
                    ShapeDrawer::drawLine(position + Vec3{ -halfSize[0], +halfSize[1], 0 }, position + Vec3{ +halfSize[0], +halfSize[1], 0 }, { 1.f, 0.f, 0.f });
                    ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], +halfSize[1], 0 }, position + Vec3{ +halfSize[0], -halfSize[1], 0 }, { 1.f, 0.f, 0.f });
                    ShapeDrawer::drawLine(position + Vec3{ +halfSize[0], -halfSize[1], 0 }, position + Vec3{ -halfSize[0], -halfSize[1], 0 }, { 1.f, 0.f, 0.f });

                    auto& anchorOffset = rectTransform->getAnchorOffset();

                    ShapeDrawer::drawLine(Vec3{ anchorOffset[0], anchorOffset[1], 0 }, Vec3{ anchorOffset[0], anchorOffset[3], 0 }, { 0.f, 0.f, 1.f });
                    ShapeDrawer::drawLine(Vec3{ anchorOffset[0], anchorOffset[3], 0 }, Vec3{ anchorOffset[2], anchorOffset[3], 0 }, { 0.f, 0.f, 1.f });
                    ShapeDrawer::drawLine(Vec3{ anchorOffset[2], anchorOffset[3], 0 }, Vec3{ anchorOffset[2], anchorOffset[1], 0 }, { 0.f, 0.f, 1.f });
                    ShapeDrawer::drawLine(Vec3{ anchorOffset[2], anchorOffset[1], 0 }, Vec3{ anchorOffset[0], anchorOffset[1], 0 }, { 0.f, 0.f, 1.f });
                }

            }
        }
    }

    //! Render camera frustum
    void EditorScene::renderCameraFrustum()
    {
        if (!isOpened())
            return;

        auto targets = Editor::getCurrentScene()->getTargets();
        const auto& target = (!targets.empty() && targets[0] != nullptr) ? targets[0] : nullptr;
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

    void EditorScene::updateCameraPosition(float touchX, float touchY)
    {
        if (m_bIsFirstTouch)
        {
            m_firstMousePosX = m_lastMousePosX = touchX;
            m_firstMousePosY = m_lastMousePosY = touchY;
            m_bIsFirstTouch = false;
            auto cameraRotation = m_currCamera->GetRotation();
            vmath_quatToEuler(cameraRotation.P(), m_cameraRotationEuler.P());
            return;
        }

        auto offset = Vec2(touchX - m_lastMousePosX, touchY - m_lastMousePosY);
        m_lastMousePosX = touchX;
        m_lastMousePosY = touchY;

        switch (m_viewTool) {
        case ViewTool::Pan:
            handleCameraPan(offset);
            break;
        case ViewTool::Orbit:
            handleCameraOrbit(offset);
            break;
        case ViewTool::FPS:
            handleCameraFPS(offset);
            break;
        case ViewTool::Zoom:
            handleCameraZoom(offset);
            break;
        default:
            handleCameraSelect(offset);
            break;
        }
    }

    void EditorScene::lookSelectedObject() {
        auto targets = Editor::getCurrentScene()->getTargets();
        const auto& target = (!targets.empty() && targets[0] != nullptr) ? targets[0] : nullptr;
        if (target && m_currCamera)
            lookAtObject(target);
    }

    void EditorScene::lookAtObject(SceneObject* object) {
        if (Editor::getInstance()->is3DCamera()) {
            if (m_cameraMoving) return;
            auto targetPos = object->getTransform()->getWorldPosition();
            m_viewSize = k_defaultViewSize; //Reset ViewSize

            auto aabb = getRenderableAABBox(object);
            Vec3 halfSize = aabb.getExtent() * 0.5f;
            if (halfSize != Vec3(0, 0)) {
                float objectSize = Vec3::Length(halfSize);
                m_viewSize = clampViewSize(objectSize);
            }

            m_cameraDistance = calcCameraViewDistance();

            //m_currCamera->SetPosition(targetPos + m_currCamera->GetCameraRotation() * Vec3(0.f, 0.f, 1.f) * m_cameraDistance);
            auto currentPos = m_currCamera->GetPosition();
            auto newPos = targetPos + m_currCamera->GetCameraRotation() * Vec3(0.f, 0.f, 1.f) * m_cameraDistance;
            auto tween = Tween::tween(currentPos, targetPos + m_currCamera->GetCameraRotation() * Vec3(0.f, 0.f, 1.f) * m_cameraDistance, 0.5f);
            m_cameraMoving = true;
            tween->onUpdate([this](Tweener* tweener) {
                m_currCamera->SetPosition(tweener->value.getVec3());
                })->onComplete([this]() { m_cameraMoving = false; });

            m_focusPosition = targetPos;
        }
        else {
            auto canvas = object->getCanvas();
            if (canvas)
            {
                auto rect = object->getRectTransform();
                if (rect)
                {
                    float viewSize = Vec2::Length(rect->getSize());
                    m_currentCanvasHeight = viewSize < 0 ? 0.001f : viewSize;
                    m_currCamera->SetOrthoHeight(m_currentCanvasHeight * 0.5f);
                    auto cameraPosition = m_currCamera->GetPosition();
                    auto pos = object->getTransform()->getWorldPosition();
                    //cameraPosition.X(pos.X());
                    //cameraPosition.Y(pos.Y());
                    //m_currCamera->SetPosition(cameraPosition);
                    pos.Z(cameraPosition.Z());
                    auto tween = Tween::tween(cameraPosition, pos + Vec3(0,1.f,0), 0.3f);
                    m_cameraMoving = true;
                    tween->onUpdate([this](Tweener* tweener) {
                        m_currCamera->SetPosition(tweener->value.getVec3());
                        })->onComplete([this]() { m_cameraMoving = false; });
                }

            } 
        }
    }

    //! Handle Camera View
    void EditorScene::handleCameraOrbit(const Vec2& offset) 
    {
        if (Editor::getInstance()->is3DCamera()) 
        {
            auto cameraPosition = m_currCamera->GetPosition();
            auto cameraRotation = m_currCamera->GetRotation();
            m_cameraDistance = Vec3::Length(cameraPosition - m_focusPosition);

            auto mouseOffset = offset * m_cameraRotationSpeed;
            m_cameraRotationEuler[1] -= mouseOffset.X();
            m_cameraRotationEuler[1] = clampEulerAngle(m_cameraRotationEuler[1]);

            m_cameraRotationEuler[0] += mouseOffset.Y();
            m_cameraRotationEuler[0] = clampEulerAngle(m_cameraRotationEuler[0]);

            Vec3 newEuler = Vec3(m_cameraRotationEuler[0], m_cameraRotationEuler[1], 0);
            Quat newQuat;
            vmath_eulerToQuat(newEuler.P(), newQuat.P());

            Vec3 negDistance = Vec3(0, 0, -m_cameraDistance);
            Vec3 fwd = Vec3::Normalize(getForwardVector(newQuat));

            Vec3 newPos = fwd * m_cameraDistance + m_focusPosition;

            m_currCamera->SetRotation(newQuat);
            m_currCamera->SetPosition(newPos);
        }
    }

    void EditorScene::handleCameraPan(const Vec2& offset)
    {
        float panDelta = m_fnKeyPressed & SYSTEM_KEYCODE_SHIFT_MASK ? 3 : 1;
        if (Editor::getInstance()->is3DCamera()) 
        {
            auto pos = m_currCamera->GetPosition();
            auto mouseOffset = offset * std::abs(std::max(0.0025f, pos.Length() / (m_currCamera->GetFarPlane() * 0.5f))) * panDelta;
            auto cameraPosition = m_currCamera->GetPosition();
            auto cameraRotation = m_currCamera->GetRotation();
            const Vec3 rightVec = { 1.f, 0.f, 0.f };
            const Vec3 upVec = { 0.f, 1.f, 0.f };
            cameraPosition -= cameraRotation * rightVec * mouseOffset.X();
            cameraPosition -= cameraRotation * upVec * mouseOffset.Y();
            m_currCamera->SetPosition(cameraPosition);

            m_resetFocus = true;
        }
        else 
        {
            // Scale with Zoom Offset
            auto zoomOffset = m_currentCanvasHeight / SystemInfo::Instance().GetGameH();
            auto mouseOffset = offset * zoomOffset * panDelta;
            auto cameraPosition = m_currCamera->GetPosition();
            auto cameraRotation = m_currCamera->GetRotation();
            const Vec3 rightVec = { 1.f, 0.f, 0.f };
            const Vec3 upVec = { 0.f, 1.f, 0.f };
            cameraPosition -= cameraRotation * rightVec * mouseOffset.X();
            cameraPosition -= cameraRotation * upVec * mouseOffset.Y();
            m_currCamera->SetPosition(cameraPosition);
        }
    }
    
    void EditorScene::handleCameraFPS(const Vec2& offset)
    {
        if (Editor::getInstance()->is3DCamera())
        {
            auto mouseOffset = offset * m_cameraRotationSpeed;
            m_cameraRotationEuler[1] -= mouseOffset.X();
            m_cameraRotationEuler[0] += mouseOffset.Y();

            Quat rot;
            vmath_eulerToQuat(m_cameraRotationEuler.P(), rot.P());
            m_currCamera->SetRotation(rot);

            m_resetFocus = true;
        }
    }
    
    void EditorScene::handleCameraScroll(const Vec2& offset) {
        float zoomDelta = m_fnKeyPressed & SYSTEM_KEYCODE_SHIFT_MASK ? 3 : 1;
        if (Editor::getInstance()->is3DCamera()) {
            float targetSize;
            auto mouseOffset = offset * m_cameraDragSpeed * zoomDelta;
            auto cameraPosition = m_currCamera->GetPosition();
            auto cameraRotation = m_currCamera->GetRotation();

            const Vec3 rightVec = { 1.f, 0.f, 0.f };
            const Vec3 upVec = { 0.f, 0.f, 1.f };
            cameraPosition += cameraRotation * rightVec * mouseOffset.X();
            cameraPosition -= cameraRotation * upVec * mouseOffset.Y();
            m_currCamera->SetPosition(cameraPosition);

            auto currentDist = Vec3::Length(m_focusPosition - cameraPosition);
            if (currentDist - mouseOffset.Y() <= 0) 
            {
                m_viewSize = k_defaultViewSize / 2;
                updateFocusPoint(true, true);
            }
            else 
            {
                targetSize = clampViewSize(m_viewSize - mouseOffset.Y());
                m_viewSize = targetSize;
                updateFocusPoint(false, false);
            }
        }
        else 
        {
            auto mouseOffset = offset * m_cameraDragSpeed * zoomDelta * 100;
            m_currentCanvasHeight -= mouseOffset.Y();
            m_currentCanvasHeight = m_currentCanvasHeight < 0 ? 0.001f : m_currentCanvasHeight;
            m_currCamera->SetOrthoHeight(m_currentCanvasHeight * 0.5f);
        }
    }

    void EditorScene::handleCameraZoom(const Vec2& offset) 
    {
        float zoomLen = Vec2::Length(offset);
        bool inverse = abs(offset.X()) > abs(offset.Y()) ? offset.X() < 0 : offset.Y() >= 0;
        if (inverse)
            zoomLen = -zoomLen;
        zoomLen = zoomLen / k_zoomFocusOffsetRate;
        auto zoomOffset = Vec2(0.f, zoomLen);
        float zoomDelta = m_fnKeyPressed & SYSTEM_KEYCODE_SHIFT_MASK ? 3 : 1;
        
        if (Editor::getInstance()->is3DCamera())
        {
            auto mouseOffset = zoomOffset * m_cameraDragSpeed * zoomDelta;
            auto cameraPosition = m_currCamera->GetPosition();
            auto cameraRotation = m_currCamera->GetRotation();
            auto currentDist = Vec3::Length(m_focusPosition - cameraPosition);
            float percent = currentDist / (m_viewSize > 4 ? 4 : m_viewSize);
            percent = percent > 50 ? 50 : percent < 0.01f ? 0.01f : percent;
            mouseOffset *= percent;
            if (currentDist - mouseOffset.Y() <= 0) mouseOffset.Y(currentDist - k_minViewSize);

            const Vec3 rightVec = { 1.f, 0.f, 0.f };
            const Vec3 fowardVec = { 0.f, 0.f, 1.f };
            cameraPosition += cameraRotation * rightVec * mouseOffset.X();
            cameraPosition -= cameraRotation * fowardVec * mouseOffset.Y();

            m_currCamera->SetPosition(cameraPosition);

            m_viewSize = clampViewSize(m_viewSize - mouseOffset.Y());
            updateFocusPoint(false, false);
        }
        else
        {
            auto mouseOffset = zoomOffset * m_cameraDragSpeed * zoomDelta * 100;
            m_currentCanvasHeight -= mouseOffset.Y();
            m_currentCanvasHeight = m_currentCanvasHeight < 0 ? 0.001f : m_currentCanvasHeight;
            m_currCamera->SetOrthoHeight(m_currentCanvasHeight * 0.5f);
        }
    }
    
    void EditorScene::renderCameraSelect()
    {
        if (m_bIsDragging && m_viewTool == ViewTool::MultiSelectArea)
        {
            auto mouseStart = Editor::getCurrentScene()->screenToClient(Vec2(m_firstMousePosX, m_firstMousePosY));
            auto mouseEnd = Editor::getCurrentScene()->screenToClient(Vec2(m_lastMousePosX, m_lastMousePosY));
            auto windowSize = Editor::getCurrentScene()->getWindowSize();

            Mat4 view;
            m_2dCamera->GetViewInverseMatrix(view);
            view = view.Inverse();

            Mat4 screen;
            m_2dCamera->GetScreenMatrix(screen);

            Mat4 proj;
            m_2dCamera->GetProjectionMatrix(proj);
            proj = screen * proj;

            ShapeDrawer::setViewProjectionMatrix2D(proj * view);
            ShapeDrawer::drawRect2D(ScreenToWorld(mouseStart, windowSize, m_2dCamera), ScreenToWorld(mouseEnd, windowSize, m_2dCamera));
        }
    }

    void EditorScene::handleCameraSelect(const Vec2& offset)
    {
        if (m_currCamera == nullptr) return;

        auto keyModifier = Editor::getApp()->getInputHandler()->getKeyboard()->getKeyModifier();
        if (!(keyModifier & (int)KeyModifier::KEY_MOD_CTRL) && !(keyModifier & (int)KeyModifier::KEY_MOD_SHIFT))
            Editor::getCurrentScene()->clearTargets();

        auto scene = Editor::getCurrentScene();
        if (scene)
        {
            auto mouseStart = scene->screenToClient(Vec2(m_firstMousePosX, m_firstMousePosY));
            auto mouseEnd = scene->screenToClient(Vec2(m_lastMousePosX, m_lastMousePosY));
            auto windowSize = Editor::getCurrentScene()->getWindowSize();

            auto minX = std::min(mouseStart[0], mouseEnd[0]);
            auto minY = std::min(mouseStart[1], mouseEnd[1]);
            auto maxX = std::max(mouseStart[0], mouseEnd[0]);
            auto maxY = std::max(mouseStart[1], mouseEnd[1]);
            auto selectRect = Vec4(minX, minY, maxX, maxY);

            for (const auto& obj : scene->getObjects())
            {
                auto objRect = AabbToScreenRect(obj->getWorldAABB(), windowSize, m_currCamera);
                objRect = Vec4(objRect[0], objRect[1], objRect[2], objRect[3]);
                if (RectInside(selectRect, objRect))
                {
                    if(keyModifier & (int)KeyModifier::KEY_MOD_CTRL)
                        Editor::getCurrentScene()->removeTarget(obj.get());
                    else
                        if (obj->getAABB().getVolume() > 0.f)
                            Editor::getCurrentScene()->addTarget(obj.get());
                }
            }
        }
    }

    //Camera Helper function
    float EditorScene::getPerspectiveCameraViewDistance(float objectSize, float fov)
    {
        //        A
        //        |\        We want to place camera at a
        //        | \       distance that, at the given FOV,
        //        |  \      would enclose a sphere of radius
        //     _..+.._\     "size". Here |BC|=size, and we
        //   .'   |   '\    need to find |AB|. ACB is a right
        //  /     |    _C   angle, andBAC is half the FOV. So
        // |      | _-   |  that gives: sin(BAC)=|BC|/|AB|,
        // |      B      |  and thus |AB|=|BC|/sin(BAC).
        // |             |
        //  \           /
        //   '._     _.'
        //      `````
        return objectSize / sin(fov * 0.5f * DEGREE_TO_RAD);
    }

    float EditorScene::calcCameraViewDistance() {
        float fov = m_currCamera != nullptr ? m_currCamera->GetFieldOfView() : k_defaultFOV;
        return getPerspectiveCameraViewDistance(m_viewSize, fov);
    }

    float EditorScene::clampEulerAngle(float angle)
    {
        if (angle < -DEG360_TO_RAD)
            angle += DEG360_TO_RAD;
        if (angle > DEG360_TO_RAD)
            angle -= DEG360_TO_RAD;
        return angle > DEG360_TO_RAD ? DEG360_TO_RAD : angle < -DEG360_TO_RAD ? -DEG360_TO_RAD : angle;
    }

    Vec3 EditorScene::getForwardVector(Quat quat) {
        Vec3 out;
        out.X(2 * (quat.X() * quat.Z() + quat.W() * quat.Y()));
        out.Y(2 * (quat.Y() * quat.Z() - quat.W() * quat.X()));
        out.Z(1 - 2 * (quat.X() * quat.X() + quat.Y() * quat.Y()));
        return out;
    }

    void EditorScene::updateFocusPoint(bool resetView, bool resetFocus)
    {
        auto cameraPosition = m_currCamera->GetPosition();
        if (resetView)
            m_cameraDistance = calcCameraViewDistance();
        else
            m_cameraDistance = Vec3::Length(m_focusPosition - cameraPosition);

        if (resetFocus) {
            auto cameraRotation = m_currCamera->GetRotation();
            auto fwd = Vec3::Normalize(getForwardVector(cameraRotation));
            m_focusPosition = cameraPosition - fwd * m_cameraDistance;
        }
    }

    float EditorScene::clampViewSize(float value) {
        return value > k_maxViewSize ? k_maxViewSize : value < k_minViewSize ? k_minViewSize : value;
    }

    AABBox EditorScene::getRenderableAABBox(SceneObject *object) 
    {
        AABBox sumAABB;
        const AABBox& aabb = object->getFrameAABB();
        sumAABB.addInternalBox(aabb);
        for (const auto child : object->getChildren()) {
            auto childAABB = getRenderableAABBox(child);
            sumAABB.addInternalBox(childAABB);
        }
        return sumAABB;
    }
}
