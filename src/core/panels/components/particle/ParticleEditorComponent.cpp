#include "core/panels/components/particle/ParticleEditorComponent.h"

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

ParticleEditorComponent::~ParticleEditorComponent()
{
    if (m_particleGroup) {
        m_particleGroup->removeAllWidgets();
        m_particleGroup->removeAllPlugins();
    }
    m_particleGroup = nullptr;
}

bool ParticleEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<Particle*>(comp);
}

void ParticleEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_particleGroup == nullptr) {
        m_particleGroup = m_group->createWidget<Group>("Particle", false);
    }

    drawParticle();

    EditorComponent::redraw();
}

void ParticleEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_particleGroup = m_group->createWidget<Group>("Particle", false);

    drawParticle();
}

void ParticleEditorComponent::drawParticle()
{
    if (m_particleGroup == nullptr)
        return;
    m_particleGroup->removeAllWidgets();

    auto particle = dynamic_cast<Particle*>(getComponent());
    if (particle == nullptr)
        return;

    auto column = m_particleGroup->createWidget<Columns<3>>();
    column->createWidget<CheckBox>("Enable", particle->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setEnabled(val);
        });
    column->createWidget<CheckBox>("Loop", particle->isLooped())->getOnDataChangedEvent().addListener([this](bool val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setLoop(val);
        });
    column->createWidget<CheckBox>("Auto Drawing", particle->isAutoDrawing())->getOnDataChangedEvent().addListener([this](bool val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setAutoDrawing(val);
        });

    std::array layer = { particle->getLayer() };
    m_particleGroup->createWidget<Drag<int>>("Layer", ImGuiDataType_U32, layer, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setLayer(val[0]);
        });

    std::array mask = { particle->getGroupMask() };
    m_particleGroup->createWidget<Drag<int>>("Group Mask", ImGuiDataType_U32, mask, 1, 0)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setGroupMask(val[0]);
        });

    std::array speed = { particle->getSpeed() };
    m_particleGroup->createWidget<Drag<float>>("Speed", ImGuiDataType_Float, speed, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setSpeed(val[0]);
        });

    std::array timeScale = { particle->getTimeScale() };
    m_particleGroup->createWidget<Drag<float>>("Time Scale", ImGuiDataType_Float, timeScale, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setTimeScale(val[0]);
        });

    std::array target = { particle->getTargetLocation().X(), particle->getTargetLocation().Y(), particle->getTargetLocation().Z() };
    m_particleGroup->createWidget<Drag<float, 3>>("Target Location", ImGuiDataType_Float, target, 0.01f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setTargetLocation({ val[0], val[1], val[2] });
        });

    std::array params = { particle->getDynamicInputParameter().X(), particle->getDynamicInputParameter().Y(), particle->getDynamicInputParameter().Z(), particle->getDynamicInputParameter().W() };
    m_particleGroup->createWidget<Drag<float, 4>>("Dynamic Parameters", ImGuiDataType_Float, params, 0.01f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setDynamicInputParameter({ val[0], val[1], val[2], val[3] });
        });

    std::array color = { particle->getColor().X(), particle->getColor().Y(), particle->getColor().Z(), particle->getColor().W() };
    m_particleGroup->createWidget<Drag<float, 4>>("Color", ImGuiDataType_Float, color, 0.001f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particle = dynamic_cast<Particle*>(getComponent());
        particle->setColor({ val[0], val[1], val[2], val[3] });
        });

    auto txtPath = m_particleGroup->createWidget<TextField>("Path", particle->getPath());
    txtPath->getOnDataChangedEvent().addListener([this](auto val) {
        TaskManager::getInstance()->addTask([val, this]() {
            auto particle = dynamic_cast<Particle*>(getComponent());
            particle->setPath(val);
            particle->play();
            });
        });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Particle))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto val) {
            TaskManager::getInstance()->addTask([val, this]() {
                auto particle = dynamic_cast<Particle*>(getComponent());
                particle->setPath(val);
                particle->play();
                dirty();
                });
            });
    }
}
NS_IGE_END