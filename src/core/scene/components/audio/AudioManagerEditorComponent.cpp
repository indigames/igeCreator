#include "core/scene/components/audio/AudioManagerEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/audio/AudioManager.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

AudioManagerEditorComponent::AudioManagerEditorComponent() {
    m_audioManagerGroup = nullptr;
}

AudioManagerEditorComponent::~AudioManagerEditorComponent()
{
    m_audioManagerGroup = nullptr;
}

void AudioManagerEditorComponent::onInspectorUpdate()
{
    drawAudioManager();
}

void AudioManagerEditorComponent::drawAudioManager()
{
    if (m_audioManagerGroup == nullptr)
        m_audioManagerGroup = m_group->createWidget<Group>("AudioManager", false);
    m_audioManagerGroup->removeAllWidgets();

    auto audioMngComp = std::dynamic_pointer_cast<AudioManager>(getComponent<CompoundComponent>()->getComponents()[0]);
    if (audioMngComp == nullptr)
        return;

    std::array volume = { audioMngComp->getGlobalVolume() };
    m_audioManagerGroup->createWidget<Drag<float>>("Global Volume", ImGuiDataType_Float, volume, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioMngComp = std::dynamic_pointer_cast<AudioManager>(getComponent<CompoundComponent>()->getComponents()[0]);
        audioMngComp->setGlobalVolume(val[0]);
    });
}

NS_IGE_END