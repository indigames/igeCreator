#include "core/panels/components/audio/AudioListenerEditorComponent.h"

#include <core/layout/Group.h>

#include "components/audio/AudioListener.h"
#include "core/widgets/Widgets.h"

NS_IGE_BEGIN

AudioListenerEditorComponent::AudioListenerEditorComponent() {
    m_audioListenerGroup = nullptr;
}

AudioListenerEditorComponent::~AudioListenerEditorComponent()
{
    m_audioListenerGroup = nullptr;
}

void AudioListenerEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_audioListenerGroup == nullptr) {
        m_audioListenerGroup = m_group->createWidget<Group>("AudioListener", false);
    }
    drawAudioListener();

    EditorComponent::redraw();
}

void AudioListenerEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_audioListenerGroup = m_group->createWidget<Group>("AudioListener", false);

    drawAudioListener();
}

void AudioListenerEditorComponent::drawAudioListener()
{
    if (m_audioListenerGroup == nullptr)
        return;
    m_audioListenerGroup->removeAllWidgets();
    auto audioListenerComp = getComponent<AudioListener>();
    if (audioListenerComp == nullptr) return;

    m_audioListenerGroup->createWidget<CheckBox>("Enable", audioListenerComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto audioListenerComp = getComponent<AudioListener>();
        audioListenerComp->setEnabled(val);
    });
}
NS_IGE_END