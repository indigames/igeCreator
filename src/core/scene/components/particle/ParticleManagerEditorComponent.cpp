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

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
    if (particleManager == nullptr)
        return;

    auto column = m_particleManagerGroup->createWidget<Columns<2>>();
    column->createWidget<CheckBox>("Culling", particleManager->isCullingEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        particleManager->setCullingEnabled(val);
        setDirty();
    });

    if (particleManager->isCullingEnabled())
    {
        std::array cullingWorld = { particleManager->getCullingWorldSize().X(), particleManager->getCullingWorldSize().Y(), particleManager->getCullingWorldSize().Z() };
        auto c1 = m_particleManagerGroup->createWidget<Drag<float, 3>>("Culling World Size", ImGuiDataType_Float, cullingWorld, 0.001f, 0.f);
        c1->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        c1->getOnDataChangedEvent().addListener([this](auto val) {
            auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
            particleManager->setCullingWorldSize({ val[0], val[1], val[2] });
        });

        std::array numLayer = { (float)particleManager->getCullingLayerNumber() };
        auto c2 = m_particleManagerGroup->createWidget<Drag<float>>("Culling Layers", ImGuiDataType_S32, numLayer, 1, 1, 8);
        c2->getOnDataBeginChangedEvent().addListener([this](auto val) {
            storeUndo();
            });
        c2->getOnDataChangedEvent().addListener([this](auto val) {
            auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
            particleManager->setCullingLayerNumber((int)val[0]);
        });
    }

    std::array maxParticles = { (float)particleManager->getMaxParticleNumber() };
    auto m1 = m_particleManagerGroup->createWidget<Drag<float>>("Max Parcicles", ImGuiDataType_S32, maxParticles, 1, 1);
    m1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    m1->getOnDataChangedEvent().addListener([this](auto val) {
        auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        particleManager->setMaxParticleNumber((int)val[0]);
    });

    std::array threadNum = { (float)particleManager->getNumberOfThreads() };
    auto n1 = m_particleManagerGroup->createWidget<Drag<float>>("Number Threads", ImGuiDataType_S32, threadNum, 1, 1, 4);
    n1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    n1->getOnDataChangedEvent().addListener([this](auto val) {
        auto particleManager = std::dynamic_pointer_cast<ParticleManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        particleManager->setNumberOfThreads((int)val[0]);
    });
}
NS_IGE_END