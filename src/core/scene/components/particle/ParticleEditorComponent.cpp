#include "core/scene/components/particle/ParticleEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "core/Editor.h"

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
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("loop", val);
    });
    column->createWidget<CheckBox>("AutoDraw", comp->getProperty<bool>("autoDraw", false))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("autoDraw", val);
    });

    auto txtPath = m_particleGroup->createWidget<TextField>("Effect", comp->getProperty<std::string>("path", ""), false, true);
    txtPath->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("path", val);
    });
    txtPath->setEndOfLine(false);
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Particle)) {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("path", GetRelativePath(path));
            setDirty();
        });
    }
    m_particleGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Particle Assets", "", { "Particle (*.efk)", "*.efk" }).result();
        if (files.size() > 0) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("path", GetRelativePath(files[0]));
            setDirty();
        }
    });

    std::array mask = { comp->getProperty<float>("mask", NAN) };
    auto g1 = m_particleGroup->createWidget<Drag<float>>("GroupMask", ImGuiDataType_U32, mask, 1, 0);
    g1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    g1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("mask", (int)val[0]);
    });

    std::array speed = { comp->getProperty<float>("speed", NAN) };
    auto s1 = m_particleGroup->createWidget<Drag<float>>("Speed", ImGuiDataType_Float, speed, 0.01f, 0.f);
    s1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    s1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("speed", val[0]);
    });

    std::array timeScale = { comp->getProperty<float>("timeScale", NAN) };
    auto t1 = m_particleGroup->createWidget<Drag<float>>("TimeScale", ImGuiDataType_Float, timeScale, 0.01f, 0.f);
    t1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    t1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("timeScale", val[0]);
    });

    auto target = comp->getProperty<Vec3>("target", { NAN, NAN, NAN });
    std::array targetArr = { target.X(), target.Y(), target.Z() };
    auto t2 = m_particleGroup->createWidget<Drag<float, 3>>("TargetPos", ImGuiDataType_Float, targetArr, 0.01f);
    t2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    t2->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("target", { val[0], val[1], val[2] });
    });

    auto params = comp->getProperty<Vec4>("param", { NAN, NAN, NAN, NAN });
    std::array paramArr = { params.X(), params.Y(), params.Z(), params.W() };
    auto p1 = m_particleGroup->createWidget<Drag<float, 4>>("Parameters", ImGuiDataType_Float, paramArr, 0.01f);
    p1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    p1->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("param", { val[0], val[1], val[2], val[3] });
    });

    auto col = comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }); 
    m_particleGroup->createWidget<Color>("Color", col)->getOnDataChangedEvent().addListener([this](const auto& val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
    });
}
NS_IGE_END