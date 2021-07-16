#include "core/scene/components/CameraEditorComponent.h"
#include "core/scene/CompoundComponent.h"
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

CameraEditorComponent::~CameraEditorComponent() {
    m_cameraCompGroup = nullptr;
}

void CameraEditorComponent::onInspectorUpdate() {
    drawCameraComponent();
}

void CameraEditorComponent::drawCameraComponent() {
    if (m_cameraCompGroup == nullptr) {
        m_cameraCompGroup = m_group->createWidget<Group>("CameraGroup", false);
    }
    m_cameraCompGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    m_cameraCompGroup->createWidget<TextField>("Name", comp->getProperty<std::string>("name", ""), true);

    // Orthographic
    m_cameraCompGroup->createWidget<CheckBox>("Ortho", comp->getProperty<bool>("ortho", false))->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("ortho", val);
        setDirty();
    });
    if (comp->getProperty<bool>("ortho", false))
    {
        auto ortColumn = m_cameraCompGroup->createWidget<Columns<2>>(180.f);
        std::array orthorW = { comp->getProperty<float>("orthoW", NAN) };
        ortColumn->createWidget<Drag<float>>("OrtW", ImGuiDataType_Float, orthorW)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("orthoW", val[0]);
        });

        std::array orthorH = { comp->getProperty<float>("orthoH", NAN) };
        ortColumn->createWidget<Drag<float>>("OrtH", ImGuiDataType_Float, orthorH)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("orthoH", val[0]);
        });
    }

    // FOV - Near - Far
    std::array fov = { comp->getProperty<float>("fov", NAN) };
    m_cameraCompGroup->createWidget<Drag<float>>("FOV", ImGuiDataType_Float, fov)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("fov", val[0]);
    });

    std::array camNear = { comp->getProperty<float>("near", NAN) };
    m_cameraCompGroup->createWidget<Drag<float>>("Near", ImGuiDataType_Float, camNear)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("near", val[0]);
    });

    std::array camFar = { comp->getProperty<float>("far", NAN) };
    m_cameraCompGroup->createWidget<Drag<float>>("Far", ImGuiDataType_Float, camFar)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("far", val[0]);
    });


    // Target    
    m_cameraCompGroup->createWidget<CheckBox>("LockTarget", comp->getProperty<bool>("lock", false))->getOnDataChangedEvent().addListener([this](auto val) {
        auto comp = getComponent<CompoundComponent>();
        if (comp == nullptr) return;
        comp->setProperty("lock", val);
        if (val) {
            auto transform = comp->getOwner()->getTransform();
            comp->setProperty("target", transform->getPosition() + Vec3(0.f, 0.f, -1.f));
        } else {
            comp->setProperty("pan", 0.f);
            comp->setProperty("tilt", 0.f);
            comp->setProperty("roll", 0.f);          
        }
        comp->setDirty();
        setDirty();
    });

    if (comp->getProperty<bool>("lock", false))
    {
        auto targetPos = comp->getProperty<Vec3>("target", { NAN, NAN, NAN });
        std::array target = { targetPos.X(), targetPos.Y(), targetPos.Z() };
        auto targetGroup = m_cameraCompGroup->createWidget<Drag<float, 3>>("Target", ImGuiDataType_Float, target);
        targetGroup->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("target", { val[0], val[1], val[2] });
            onTransformChanged();
        });
    }
    else
    {
        // Pan - Tilt - Roll
        std::array pan = { RADIANS_TO_DEGREES(comp->getProperty<float>("pan", NAN)) };
        m_cameraCompGroup->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("pan", DEGREES_TO_RADIANS(val[0]));
            onTransformChanged();
        });
        std::array tilt = { RADIANS_TO_DEGREES(comp->getProperty<float>("tilt", NAN)) };
        m_cameraCompGroup->createWidget<Drag<float>>("Tilt", ImGuiDataType_Float, tilt)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("tilt", DEGREES_TO_RADIANS(val[0]));
            onTransformChanged();
        });
        std::array roll = { RADIANS_TO_DEGREES(comp->getProperty<float>("roll", NAN)) };
        m_cameraCompGroup->createWidget<Drag<float>>("Roll", ImGuiDataType_Float, roll)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("roll", DEGREES_TO_RADIANS(val[0]));
            onTransformChanged();
        });
    }

    // Width Based
    m_cameraCompGroup->createWidget<CheckBox>("wBase", comp->getProperty<bool>("wBase", false))->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("wBase", val);
    });

    // Aspect Ratio
    std::array ratio = { comp->getProperty<float>("aspect", NAN) };
    m_cameraCompGroup->createWidget<Drag<float>>("Aspect", ImGuiDataType_Float, ratio)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("aspect", val[0]);
    });

    // Up vector
    std::array upArr = { comp->getProperty<float>("up", NAN) };
    m_cameraCompGroup->createWidget<Drag<float>>("Up", ImGuiDataType_S32, upArr, 1, 0, 2)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("up", (int)val[0]);
    });

    // Screen scale
    auto sScale = comp->getProperty<Vec2>("scrScale", {NAN, NAN});
    std::array scrScale = { sScale[0], sScale[1] };
    m_cameraCompGroup->createWidget<Drag<float, 2>>("ScrScale", ImGuiDataType_Float, scrScale)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("scrScale", Vec2(val[0], val[1]));
    });

    // Screen offset
    auto sOff = comp->getProperty<Vec2>("scrOff", { NAN, NAN });
    std::array scrOffset = { sOff[0], sOff[1] };
    m_cameraCompGroup->createWidget<Drag<float, 2>>("scrOffset", ImGuiDataType_Float, scrOffset)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("scrOff", Vec2(val[0], val[1]));
    });

    // Screen radian
    auto sRad = comp->getProperty<float>("scrRad", NAN);
    std::array scrRad = { sRad };
    m_cameraCompGroup->createWidget<Drag<float>>("scrRad", ImGuiDataType_Float, scrRad)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("scrRad", val[0]);
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