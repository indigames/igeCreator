#include "core/scene/components/particle/ParticleEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/particle/Particle.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

#include "core/task/TaskManager.h"
NS_IGE_BEGIN

ParticleEditorComponent::ParticleEditorComponent() {
    m_particleGroup = nullptr;
}

ParticleEditorComponent::~ParticleEditorComponent() {
    m_particleGroup = nullptr;
}

void ParticleEditorComponent::onInspectorUpdate() {
    drawParticle();
}

void ParticleEditorComponent::drawParticle() {
    if (m_particleGroup == nullptr)
        m_particleGroup = m_group->createWidget<Group>("Particle", false);;
    m_particleGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto column = m_particleGroup->createWidget<Columns<2>>();
    column->createWidget<CheckBox>("Loop", comp->getProperty<bool>("loop", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("loop", val);
    });
    column->createWidget<CheckBox>("AutoDraw", comp->getProperty<bool>("autoDraw", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("autoDraw", val);
    });

    std::array layer = { comp->getProperty<int>("layer", 0) };
    m_particleGroup->createWidget<Drag<int>>("Layer", ImGuiDataType_U32, layer, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("layer", val);
    });

    std::array mask = { comp->getProperty<int>("mask", 0) };
    m_particleGroup->createWidget<Drag<int>>("GroupMask", ImGuiDataType_U32, mask, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("mask", val);
    });

    std::array speed = { comp->getProperty<float>("speed", 0) };
    m_particleGroup->createWidget<Drag<float>>("Speed", ImGuiDataType_Float, speed, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("speed", val);
    });

    std::array timeScale = { comp->getProperty<float>("timeScale", 0) };
    m_particleGroup->createWidget<Drag<float>>("TimeScale", ImGuiDataType_Float, timeScale, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("timeScale", val);
    });

    auto target = comp->getProperty<Vec3>("target", {});
    std::array targetArr = { target.X(), target.Y(), target.Z() };
    m_particleGroup->createWidget<Drag<float, 3>>("TargetPos", ImGuiDataType_Float, targetArr, 0.01f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("target", { val[0], val[1], val[2] });
    });

    auto params = comp->getProperty<Vec4>("param", {});
    std::array paramArr = { params.X(), params.Y(), params.Z(), params.W() };
    m_particleGroup->createWidget<Drag<float, 4>>("Parameters", ImGuiDataType_Float, paramArr, 0.01f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("param", { val[0], val[1], val[2], val[3] });
    });

    auto col = comp->getProperty<Vec4>("color", {});
    std::array color = { col.X(), col.Y(), col.Z(), col.W() };
    m_particleGroup->createWidget<Drag<float, 4>>("Color", ImGuiDataType_Float, color, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
    });

    auto txtPath = m_particleGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""));
    txtPath->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("path", val);
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Particle))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("path", val);
            setDirty();
        });
    }
}
NS_IGE_END