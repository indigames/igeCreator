#include "core/scene/components/particle/ParticleManagerEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/particle/ParticleManager.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

ParticleManagerEditorComponent::ParticleManagerEditorComponent() {
    m_particleManagerGroup = nullptr;
}

ParticleManagerEditorComponent::~ParticleManagerEditorComponent() {
    m_particleManagerGroup = nullptr;
}

void ParticleManagerEditorComponent::onInspectorUpdate() {
    drawParticleManager();
}

void ParticleManagerEditorComponent::drawParticleManager()
{
    if (m_particleManagerGroup == nullptr)
        m_particleManagerGroup = m_group->createWidget<Group>("ParticleManager", false);;
    m_particleManagerGroup->removeAllWidgets();

    auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
    if (particleManager == nullptr)
        return;

    auto column = m_particleManagerGroup->createWidget<Columns<2>>();
    column->createWidget<CheckBox>("Culling", particleManager->isCullingEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        particleManager->setCullingEnabled(val);
        setDirty();
    });

    if (particleManager->isCullingEnabled())
    {
        std::array cullingWorld = { particleManager->getCullingWorldSize().X(), particleManager->getCullingWorldSize().Y(), particleManager->getCullingWorldSize().Z() };
        m_particleManagerGroup->createWidget<Drag<float, 3>>("Culling World Size", ImGuiDataType_Float, cullingWorld, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
            particleManager->setCullingWorldSize({ val[0], val[1], val[2] });
        });

        std::array numLayer = { particleManager->getCullingLayerNumber() };
        m_particleManagerGroup->createWidget<Drag<int>>("Number Layers", ImGuiDataType_S32, numLayer, 1, 1, 8)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
            particleManager->setCullingLayerNumber(val[0]);
        });
    }

    std::array maxParticles = { particleManager->getMaxParticleNumber() };
    m_particleManagerGroup->createWidget<Drag<int>>("Max Parcicles", ImGuiDataType_S32, maxParticles, 1, 1)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        particleManager->setMaxParticleNumber(val[0]);
    });

    std::array threadNum = { particleManager->getNumberOfThreads() };
    m_particleManagerGroup->createWidget<Drag<int>>("Number Threads", ImGuiDataType_S32, threadNum, 1, 1, 4)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        particleManager->setNumberOfThreads(val[0]);
    });
}
NS_IGE_END