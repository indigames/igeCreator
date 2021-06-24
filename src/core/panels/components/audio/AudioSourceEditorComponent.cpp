#include "core/panels/components/audio/AudioSourceEditorComponent.h"

#include <core/layout/Group.h>

#include "components/audio/AudioSource.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

AudioSourceEditorComponent::AudioSourceEditorComponent() {
    m_audioSourceGroup = nullptr;
}

AudioSourceEditorComponent::~AudioSourceEditorComponent()
{
    if (m_audioSourceGroup) {
        m_audioSourceGroup->removeAllWidgets();
        m_audioSourceGroup->removeAllPlugins();
    }
    m_audioSourceGroup = nullptr;
}

bool AudioSourceEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<AudioSource*>(comp);
}

void AudioSourceEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_audioSourceGroup == nullptr) {
        m_audioSourceGroup = m_group->createWidget<Group>("AudioSource", false);
    }
    drawAudioSource();

    EditorComponent::redraw();
}

void AudioSourceEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_audioSourceGroup = m_group->createWidget<Group>("AudioSource", false);

    drawAudioSource();
}

void AudioSourceEditorComponent::drawAudioSource()
{
    if (m_audioSourceGroup == nullptr)
        return;
    m_audioSourceGroup->removeAllWidgets();

    auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
    if (audioSourceComp == nullptr)
        return;

    auto columns = m_audioSourceGroup->createWidget<Columns<3>>();
    columns->createWidget<CheckBox>("Enable", audioSourceComp->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setEnabled(val);
        });
    columns->createWidget<CheckBox>("PlayOnEnable", audioSourceComp->getPlayOnEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setPlayOnEnabled(val);
        });
    columns->createWidget<Label>(""); // Empty node
    columns->createWidget<CheckBox>("Stream", audioSourceComp->isStream())->getOnDataChangedEvent().addListener([this](bool val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setStream(val);
        });
    columns->createWidget<CheckBox>("Loop", audioSourceComp->isLooped())->getOnDataChangedEvent().addListener([this](bool val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setLoop(val);
        });
    columns->createWidget<CheckBox>("Single", audioSourceComp->isSingleInstance())->getOnDataChangedEvent().addListener([this](bool val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setSingleInstance(val);
        });

    std::array volume = { audioSourceComp->getVolume() };
    m_audioSourceGroup->createWidget<Drag<float>>("Volume", ImGuiDataType_Float, volume, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setVolume(val[0]);
        });

    std::array pan = { audioSourceComp->getPan() };
    m_audioSourceGroup->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan, 0.01f, -1.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setPan(val[0]);
        });

    std::array minDistance = { audioSourceComp->getMinDistance() };
    m_audioSourceGroup->createWidget<Drag<float>>("Min Distance", ImGuiDataType_Float, minDistance, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setMinDistance(val[0]);
        });

    std::array maxDistance = { audioSourceComp->getMaxDistance() };
    m_audioSourceGroup->createWidget<Drag<float>>("Max Distance", ImGuiDataType_Float, maxDistance, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setMaxDistance(val[0]);
        });

    auto vel = audioSourceComp->getVelocity();
    std::array velocity = { vel[0], vel[1], vel[2] };
    m_audioSourceGroup->createWidget<Drag<float, 3>>("Velocity", ImGuiDataType_Float, velocity, 0.01f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setVelocity({ val[0], val[1], val[2] });
        });

    std::array attenuationModel = { audioSourceComp->getAttenuationModel() };
    m_audioSourceGroup->createWidget<Drag<int>>("Attenuation Model", ImGuiDataType_S32, attenuationModel, 1, 0, SoLoud::AudioSource::ATTENUATION_MODELS::EXPONENTIAL_DISTANCE)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setAttenuationModel(val[0]);
        });

    std::array attenuationFactor = { audioSourceComp->getAttenuationRollOffFactor() };
    m_audioSourceGroup->createWidget<Drag<float>>("Attenuation Factor", ImGuiDataType_Float, attenuationFactor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setAttenuationRollOffFactor(val[0]);
        });

    std::array dopplerFactor = { audioSourceComp->getDopplerFactor() };
    m_audioSourceGroup->createWidget<Drag<float>>("Doppler Factor", ImGuiDataType_Float, dopplerFactor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setDopplerFactor(val[0]);
        });

    auto txtPath = m_audioSourceGroup->createWidget<TextField>("Path", audioSourceComp->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
        audioSourceComp->setPath(txt);
        });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Audio))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
            audioSourceComp->setPath(txt);
            dirty();
            });
    }
    m_audioSourceGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "WAV audio file (*.wav)", "*.wav", "OGG audio file (*.ogg)", "*.ogg" , "MP3 audio file (*.mp3)", "*.mp3" }).result();
        if (files.size() > 0)
        {
            auto audioSourceComp = dynamic_cast<AudioSource*>(getComponent());
            audioSourceComp->setPath(files[0]);
            dirty();
        }
        });
}
NS_IGE_END