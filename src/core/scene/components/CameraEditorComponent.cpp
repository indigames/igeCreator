#include "core/scene/components/CameraEditorComponent.h"

#include "components/CameraComponent.h"
#include <core/layout/Group.h>

#include "core/layout/Columns.h"
#include "core/widgets/Widgets.h"

#include "core/task/TaskManager.h"

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/InspectorEditor.h"

NS_IGE_BEGIN

CameraEditorComponent::CameraEditorComponent() {
    m_cameraCompGroup = nullptr;
}

CameraEditorComponent::~CameraEditorComponent()
{
    m_cameraLockTargetGroup = nullptr;
    m_cameraCompGroup = nullptr;
}

void CameraEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_cameraCompGroup == nullptr)
        m_cameraCompGroup = m_group->createWidget<Group>("CameraGroup", false);
    drawCameraComponent();

    EditorComponent::redraw();
}

void CameraEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    auto camera = getComponent<CameraComponent>();
    if (camera == nullptr)
        return;

    m_cameraCompGroup = m_group->createWidget<Group>("CameraGroup", false);
    drawCameraComponent();
}

void CameraEditorComponent::drawCameraComponent()
{
    if (m_cameraCompGroup == nullptr)
        return;
    auto camera = getComponent<CameraComponent>();
    if (camera == nullptr)
        return;
    m_cameraCompGroup->removeAllWidgets();

    auto columns = m_cameraCompGroup->createWidget<Columns<3>>(120.f);
    // Orthographic
    std::array orthorW = { camera->getOrthoWidth() };
    columns->createWidget<Drag<float>>("OrtW", ImGuiDataType_Float, orthorW)->getOnDataChangedEvent().addListener([this](auto val) {
        auto camera = getComponent<CameraComponent>();
        camera->setOrthoWidth(val[0]);
        });
    std::array orthorH = { camera->getOrthoHeight() };
    columns->createWidget<Drag<float>>("OrtH", ImGuiDataType_Float, orthorH)->getOnDataChangedEvent().addListener([this](auto val) {
        auto camera = getComponent<CameraComponent>();
        camera->setOrthoHeight(val[0]);
        });
    columns->createWidget<CheckBox>("IsOrtho", camera->isOrthoProjection())->getOnDataChangedEvent().addListener([this](auto val) {
        auto camera = getComponent<CameraComponent>();
        camera->setOrthoProjection(val);
        });

    // FOV - Near - Far
    std::array fov = { camera->getFieldOfView() };
    columns->createWidget<Drag<float>>("FOV", ImGuiDataType_Float, fov)->getOnDataChangedEvent().addListener([this](auto val) {
        auto camera = getComponent<CameraComponent>();
        camera->setFieldOfView(val[0]);
        });
    std::array camNear = { camera->getNearPlane() };
    columns->createWidget<Drag<float>>("Near", ImGuiDataType_Float, camNear)->getOnDataChangedEvent().addListener([this](auto val) {
        auto camera = getComponent<CameraComponent>();
        camera->setNearPlane(val[0]);
        });
    std::array camFar = { camera->getFarPlane() };
    columns->createWidget<Drag<float>>("Far", ImGuiDataType_Float, camFar)->getOnDataChangedEvent().addListener([this](auto val) {
        auto camera = getComponent<CameraComponent>();
        camera->setFarPlane(val[0]);
        });

    // Target
    auto drawCameraLockTarget = [this]() {
        m_cameraLockTargetGroup->removeAllWidgets();
        auto camera = getComponent<CameraComponent>();
        if (camera == nullptr) return;
        if (camera->getLockOn())
        {
            std::array target = { camera->getTarget().X(), camera->getTarget().Y(), camera->getTarget().Z() };
            auto targetGroup = m_cameraLockTargetGroup->createWidget<Drag<float, 3>>("Target", ImGuiDataType_Float, target);
            targetGroup->getOnDataChangedEvent().addListener([this](auto val) {
                auto camera = getComponent<CameraComponent>();
                camera->setTarget({ val[0], val[1], val[2] });
                onTransformChanged();
            });
        }
        else
        {
            // Pan - Tilt - Roll
            auto columns = m_cameraLockTargetGroup->createWidget<Columns<3>>(120.f);
            std::array pan = { RADIANS_TO_DEGREES(camera->getPan()) };
            columns->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan)->getOnDataChangedEvent().addListener([this](auto val) {
                auto camera = getComponent<CameraComponent>();
                camera->setPan(DEGREES_TO_RADIANS(val[0]));
                onTransformChanged();
            });
            std::array tilt = { RADIANS_TO_DEGREES(camera->getTilt()) };
            columns->createWidget<Drag<float>>("Tilt", ImGuiDataType_Float, tilt)->getOnDataChangedEvent().addListener([this](auto val) {
                auto camera = getComponent<CameraComponent>();
                camera->setTilt(DEGREES_TO_RADIANS(val[0]));
                onTransformChanged();
            });
            std::array roll = { RADIANS_TO_DEGREES(camera->getRoll()) };
            columns->createWidget<Drag<float>>("Roll", ImGuiDataType_Float, roll)->getOnDataChangedEvent().addListener([this](auto val) {
                auto camera = getComponent<CameraComponent>();
                camera->setRoll(DEGREES_TO_RADIANS(val[0]));
                onTransformChanged();
            });
        }
    };

    m_cameraCompGroup->createWidget<CheckBox>("LockTarget", camera->getLockOn())->getOnDataChangedEvent().addListener([drawCameraLockTarget, this](auto locked) {
        auto camera = getComponent<CameraComponent>();
        if (camera == nullptr) return;
        if (!locked)
        {
            auto transform = camera->getOwner()->getTransform();
            camera->setTarget(transform->getPosition() + Vec3(0.f, 0.f, -1.f));
        }
        else
        {
            camera->setPan(0.f);
            camera->setTilt(0.f);
            camera->setRoll(0.f);
        }
        camera->lockOnTarget(locked);
        onTransformChanged();
        drawCameraLockTarget();
        });

    m_cameraLockTargetGroup = m_cameraCompGroup->createWidget<Group>("LockTargetGroup", false);
    drawCameraLockTarget();

    // Width Based
    auto widthBasedColumns = m_cameraCompGroup->createWidget<Columns<2>>(180.f);
    widthBasedColumns->createWidget<CheckBox>("WidthBased", camera->isWidthBase())->getOnDataChangedEvent().addListener([this](auto val) {
        auto camera = getComponent<CameraComponent>();
        camera->setWidthBase(val);
    });

    // Aspect Ratio
    std::array ratio = { camera->getAspectRatio() };
    widthBasedColumns->createWidget<Drag<float>>("Ratio", ImGuiDataType_Float, ratio)->getOnDataChangedEvent().addListener([this](auto val) {
        auto camera = getComponent<CameraComponent>();
        camera->setAspectRatio(val[0]);
    });
}

void CameraEditorComponent::onTransformChanged()
{
    auto camera = getComponent<CameraComponent>();
    if (camera != nullptr)
    {
        auto m_transformComponent = camera->getOwner()->getTransform();
        Editor::getCanvas()->getInspector()->getInspectorEditor()->makeDirty(m_transformComponent->getInstanceId());
    }
}

NS_IGE_END