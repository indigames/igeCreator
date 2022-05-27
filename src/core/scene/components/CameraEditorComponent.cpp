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
#include "core/panels/Hierarchy.h"
#include "core/panels/EditorScene.h"
#include "core/panels/Inspector.h"

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


    // FOV - Near - Far
    std::array fov = { comp->getProperty<float>("fov", NAN) };
    auto fovE = m_cameraCompGroup->createWidget<Drag<float>>("FOV", ImGuiDataType_Float, fov);
    fovE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    fovE->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("fov", val[0]);
    });

    std::array camNear = { comp->getProperty<float>("near", NAN) };
    auto camNearE = m_cameraCompGroup->createWidget<Drag<float>>("Near", ImGuiDataType_Float, camNear);
    camNearE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    camNearE->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("near", val[0]);
    });

    std::array camFar = { comp->getProperty<float>("far", NAN) };
    auto camFarE = m_cameraCompGroup->createWidget<Drag<float>>("Far", ImGuiDataType_Float, camFar);
    camFarE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    camFarE->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("far", val[0]);
    });

    // Aspect Ratio
    std::array ratio = { comp->getProperty<float>("aspect", NAN) };
    auto ratioE = m_cameraCompGroup->createWidget<Drag<float>>("Aspect", ImGuiDataType_Float, ratio);
    ratioE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
     });
    ratioE->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("aspect", val[0]);
    });

    // Up vector
    std::array upArr = { comp->getProperty<float>("up", NAN) };
    auto upArrE = m_cameraCompGroup->createWidget<Drag<float>>("Up", ImGuiDataType_S32, upArr, 1, 0, 2);
    upArrE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    upArrE->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("up", (int)val[0]);
    });

    // Orthographic
    m_cameraCompGroup->createWidget<CheckBox>("Ortho", comp->getProperty<bool>("ortho", false))->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("ortho", val);
        setDirty();
    });
    if (comp->getProperty<bool>("ortho", false))
    {
        std::array orthorW = { comp->getProperty<float>("orthoW", NAN) };
        m_cameraCompGroup->createWidget<Drag<float>>("OrtW", ImGuiDataType_Float, orthorW)->getOnDataChangedEvent().addListener([this](auto val) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("orthoW", val[0]);
        });

        std::array orthorH = { comp->getProperty<float>("orthoH", NAN) };
        m_cameraCompGroup->createWidget<Drag<float>>("OrtH", ImGuiDataType_Float, orthorH)->getOnDataChangedEvent().addListener([this](auto val) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("orthoH", val[0]);
        });
    }

    // Target    
    m_cameraCompGroup->createWidget<CheckBox>("LockTarget", comp->getProperty<bool>("lock", false))->getOnDataChangedEvent().addListener([this](auto val) {
        auto comp = getComponent<CompoundComponent>();
        if (comp == nullptr) return;
        storeUndo();
        comp->setProperty("lock", val);        
        comp->setDirty();
        setDirty();
    });

    if (comp->getProperty<bool>("lock", false))
    {
        auto targetPos = comp->getProperty<Vec3>("target", { NAN, NAN, NAN });
        std::array target = { targetPos.X(), targetPos.Y(), targetPos.Z() };
        auto targetGroup = m_cameraCompGroup->createWidget<Drag<float, 3>>("Target", ImGuiDataType_Float, target);
        targetGroup->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
        });
        targetGroup->getOnDataChangedEvent().addListener([this](auto val) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("target", { val[0], val[1], val[2] });
            onTransformChanged();
        });
    }

    // Width Based
    m_cameraCompGroup->createWidget<CheckBox>("wBase", comp->getProperty<bool>("wBase", false))->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("wBase", val);
    });

    // Screen scale
    auto sScale = comp->getProperty<Vec2>("scrScale", {NAN, NAN});
    std::array scrScale = { sScale[0], sScale[1] };
    auto srcScaleE = m_cameraCompGroup->createWidget<Drag<float, 2>>("ScrScale", ImGuiDataType_Float, scrScale);
    srcScaleE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    srcScaleE->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("scrScale", Vec2(val[0], val[1]));
    });

    // Screen offset
    auto sOff = comp->getProperty<Vec2>("scrOff", { NAN, NAN });
    std::array scrOffset = { sOff[0], sOff[1] };
    auto srcOffsetE = m_cameraCompGroup->createWidget<Drag<float, 2>>("ScrOffset", ImGuiDataType_Float, scrOffset);
    srcOffsetE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
     });
    srcOffsetE->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("scrOff", Vec2(val[0], val[1]));
    });

    // Screen radian
    auto sRad = comp->getProperty<float>("scrRad", NAN);
    std::array scrRad = { sRad };
    auto srcRadE = m_cameraCompGroup->createWidget<Drag<float>>("ScrRot", ImGuiDataType_Float, scrRad);
    srcRadE->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    srcRadE->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("scrRad", val[0]);
    });



    auto color = comp->getProperty<Vec4>("clearColor", Vec4(1.f, 1.f, 1.f, 1.f));
    auto colorVec = Vec4(color[0], color[1], color[2], color[3]);
    auto cColorE = m_cameraCompGroup->createWidget<Color>("ClearColor", colorVec);
    cColorE->getOnDataChangedEvent().addListener([this](const auto& val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("clearColor", { val[0], val[1], val[2], val[3] });
    });
}

void CameraEditorComponent::onTransformChanged()
{
    auto camera = getComponent<CameraComponent>();
    if (camera != nullptr)
    {
        auto m_transformComponent = camera->getOwner()->getTransform();
        Editor::getCanvas()->getInspector()->getInspectorEditor()->makeDirty(Component::Type::Transform);
    }
}

NS_IGE_END