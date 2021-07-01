#include "core/panels/components/audio/AudioManagerEditorComponent.h"

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

void AudioManagerEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_audioManagerGroup == nullptr) {
        m_audioManagerGroup = m_group->createWidget<Group>("AudioManager", false);
    }
    drawAudioManager();

    EditorComponent::redraw();
}

void AudioManagerEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_audioManagerGroup = m_group->createWidget<Group>("AudioManager", false);

    drawAudioManager();
}

void AudioManagerEditorComponent::drawAudioManager()
{
    if (m_audioManagerGroup == nullptr)
        return;
    m_audioManagerGroup->removeAllWidgets();

    auto audioMngComp = getComponent<AudioManager>();
    if (audioMngComp == nullptr)
        return;

    std::array volume = { audioMngComp->getGlobalVolume() };
    m_audioManagerGroup->createWidget<Drag<float>>("Global Volume", ImGuiDataType_Float, volume, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioMngComp = getComponent<AudioManager>();
        audioMngComp->setGlobalVolume(val[0]);
    });
}

NS_IGE_END