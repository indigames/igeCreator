#include "core/panels/components/particle/ParticleManagerEditorComponent.h"

#include <core/layout/Group.h>

#include "components/particle/ParticleManager.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

ParticleManagerEditorComponent::ParticleManagerEditorComponent() {
    m_particleManagerGroup = nullptr;
}

ParticleManagerEditorComponent::~ParticleManagerEditorComponent()
{
    if (m_particleManagerGroup) {
        m_particleManagerGroup->removeAllWidgets();
        m_particleManagerGroup->removeAllPlugins();
    }
    m_particleManagerGroup = nullptr;
}

bool ParticleManagerEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<ParticleManager*>(comp);
}

void ParticleManagerEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_particleManagerGroup == nullptr) {
        m_particleManagerGroup = m_group->createWidget<Group>("ParticleManager", false);
    }

    drawParticleManager();

    EditorComponent::redraw();
}

void ParticleManagerEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_particleManagerGroup = m_group->createWidget<Group>("ParticleManager", false);

    drawParticleManager();
}

void ParticleManagerEditorComponent::drawParticleManager()
{
    if (m_particleManagerGroup == nullptr)
        return;
    m_particleManagerGroup->removeAllWidgets();

    auto particleManager = m_targetObject->getComponent<ParticleManager>();
    if (particleManager == nullptr)
        return;

    auto column = m_particleManagerGroup->createWidget<Columns<2>>();
    column->createWidget<CheckBox>("Culling", particleManager->isCullingEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto particleManager = m_targetObject->getComponent<ParticleManager>();
        particleManager->setCullingEnabled(val);
        m_bisDirty = true;
        });

    if (particleManager->isCullingEnabled())
    {
        std::array cullingWorld = { particleManager->getCullingWorldSize().X(), particleManager->getCullingWorldSize().Y(), particleManager->getCullingWorldSize().Z() };
        m_particleManagerGroup->createWidget<Drag<float, 3>>("Culling World Size", ImGuiDataType_Float, cullingWorld, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particleManager = m_targetObject->getComponent<ParticleManager>();
            particleManager->setCullingWorldSize({ val[0], val[1], val[2] });
            });

        std::array numLayer = { particleManager->getCullingLayerNumber() };
        m_particleManagerGroup->createWidget<Drag<int>>("Number Layers", ImGuiDataType_S32, numLayer, 1, 1, 8)->getOnDataChangedEvent().addListener([this](auto val) {
            auto particleManager = m_targetObject->getComponent<ParticleManager>();
            particleManager->setCullingLayerNumber(val[0]);
            });
    }

    std::array maxParticles = { particleManager->getMaxParticleNumber() };
    m_particleManagerGroup->createWidget<Drag<int>>("Max Parcicles", ImGuiDataType_S32, maxParticles, 1, 1)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particleManager = m_targetObject->getComponent<ParticleManager>();
        particleManager->setMaxParticleNumber(val[0]);
        });

    std::array threadNum = { particleManager->getNumberOfThreads() };
    m_particleManagerGroup->createWidget<Drag<int>>("Number Threads", ImGuiDataType_S32, threadNum, 1, 1, 4)->getOnDataChangedEvent().addListener([this](auto val) {
        auto particleManager = m_targetObject->getComponent<ParticleManager>();
        particleManager->setNumberOfThreads(val[0]);
        });
}
NS_IGE_END