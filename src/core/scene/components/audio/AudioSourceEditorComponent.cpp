#include "core/scene/components/audio/AudioSourceEditorComponent.h"
#include "core/scene/CompoundComponent.h"

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

AudioSourceEditorComponent::~AudioSourceEditorComponent() {
    m_audioSourceGroup = nullptr;
}

void AudioSourceEditorComponent::onInspectorUpdate() {
    drawAudioSource();
}

void AudioSourceEditorComponent::drawAudioSource() {
    if (m_audioSourceGroup == nullptr)
        m_audioSourceGroup = m_group->createWidget<Group>("AudioSource", false);
    m_audioSourceGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto columns = m_audioSourceGroup->createWidget<Columns<2>>();
    columns->createWidget<CheckBox>("AutoPlay", comp->getProperty<bool>("play", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("play", val);
    });
    columns->createWidget<CheckBox>("Single", comp->getProperty<bool>("single", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("single", val);
    });
    columns->createWidget<CheckBox>("Stream", comp->getProperty<bool>("stream", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("stream", val);
    });
    columns->createWidget<CheckBox>("Loop", comp->getProperty<bool>("loop", true))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("loop", val);
    });


    std::array volume = { comp->getProperty<float>("volume", 1.f) };
    m_audioSourceGroup->createWidget<Drag<float>>("Volume", ImGuiDataType_Float, volume, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("volume", val);
    });

    std::array pan = { comp->getProperty<float>("pan", 0.f) };
    m_audioSourceGroup->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan, 0.01f, -1.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("pan", val);
    });

    std::array minDistance = { comp->getProperty<float>("minDist", 0.f) };
    m_audioSourceGroup->createWidget<Drag<float>>("Min Distance", ImGuiDataType_Float, minDistance, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("minDist", val);
    });

    std::array maxDistance = { comp->getProperty<float>("maxDist", 0.f) };
    m_audioSourceGroup->createWidget<Drag<float>>("Max Distance", ImGuiDataType_Float, maxDistance, 0.01f, 0.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("maxDist", val);
    });

    auto vel = comp->getProperty<Vec3>("velocity", {0.f, 0.f, 0.f});
    std::array velocity = { vel[0], vel[1], vel[2] };
    m_audioSourceGroup->createWidget<Drag<float, 3>>("Velocity", ImGuiDataType_Float, velocity, 0.01f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("velocity", { val[0], val[1], val[2] });
    });

    auto attenuationModel = m_audioSourceGroup->createWidget<ComboBox>("Attenuation Model", comp->getProperty<int>("aBlendOp", 0));
    attenuationModel->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("attModel", val);
    });
    attenuationModel->setEndOfLine(false);
    attenuationModel->addChoice((int)SoLoud::AudioSource::ATTENUATION_MODELS::NO_ATTENUATION, "NO ATTENUATION");
    attenuationModel->addChoice((int)SoLoud::AudioSource::ATTENUATION_MODELS::INVERSE_DISTANCE, "INVERSE DISTANCE");
    attenuationModel->addChoice((int)SoLoud::AudioSource::ATTENUATION_MODELS::LINEAR_DISTANCE, "LINEAR DISTANCE");
    attenuationModel->addChoice((int)SoLoud::AudioSource::ATTENUATION_MODELS::EXPONENTIAL_DISTANCE, "EXPONENTIAL DISTANCE");
    attenuationModel->setEndOfLine(true);

    std::array attenuationFactor = { comp->getProperty<float>("attFactor", 0.f) };
    m_audioSourceGroup->createWidget<Drag<float>>("Attenuation Factor", ImGuiDataType_Float, attenuationFactor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("attFactor", val);
    });

    std::array dopplerFactor = { comp->getProperty<float>("dopFactor", 0.f) };
    m_audioSourceGroup->createWidget<Drag<float>>("Doppler Factor", ImGuiDataType_Float, dopplerFactor, 0.01f, 0.f, 1.f)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("dopFactor", val);
    });

    auto txtPath = m_audioSourceGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""));
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        getComponent<CompoundComponent>()->setProperty("path", txt);
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Audio))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            getComponent<CompoundComponent>()->setProperty("path", txt);
            setDirty();
        });
    }
    m_audioSourceGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "WAV audio file (*.wav)", "*.wav", "OGG audio file (*.ogg)", "*.ogg" , "MP3 audio file (*.mp3)", "*.mp3" }).result();
        if (files.size() > 0) {
            getComponent<CompoundComponent>()->setProperty("path", files[0]);
            setDirty();
        }
    });
}
NS_IGE_END