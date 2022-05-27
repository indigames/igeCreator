#include "core/scene/components/audio/AudioSourceEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "core/Editor.h"

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
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("play", val);
    });
    columns->createWidget<CheckBox>("Single", comp->getProperty<bool>("single", true))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("single", val);
    });
    columns->createWidget<CheckBox>("Stream", comp->getProperty<bool>("stream", true))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("stream", val);
    });
    columns->createWidget<CheckBox>("Loop", comp->getProperty<bool>("loop", true))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("loop", val);
    });

    auto txtPath = m_audioSourceGroup->createWidget<TextField>("Track", comp->getProperty<std::string>("path", ""), false, true);
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("path", txt);
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Audio)) {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("path", GetRelativePath(path));
            setDirty();
        });
    }
    m_audioSourceGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "WAV audio file (*.wav)", "*.wav", "OGG audio file (*.ogg)", "*.ogg" , "MP3 audio file (*.mp3)", "*.mp3", "MP4 audio file (*.mp4)", "*.mp4" }).result();
        if (files.size() > 0) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("path", GetRelativePath(files[0]));
            setDirty();
        }
    });

    std::array volume = { comp->getProperty<float>("volume", NAN) };
    auto v1 = m_audioSourceGroup->createWidget<Drag<float>>("Volume", ImGuiDataType_Float, volume, 0.01f, 0.f, 1.f);
    v1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    v1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("volume", val[0]);
    });

    std::array pan = { comp->getProperty<float>("pan", NAN) };
    auto p1 = m_audioSourceGroup->createWidget<Drag<float>>("Pan", ImGuiDataType_Float, pan, 0.01f, -1.f, 1.f);
    p1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    p1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("pan", val[0]);
    });

    std::array minDistance = { comp->getProperty<float>("minDist", NAN) };
    auto m1 = m_audioSourceGroup->createWidget<Drag<float>>("Min Distance", ImGuiDataType_Float, minDistance, 0.01f, 0.f);
    m1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    m1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("minDist", val[0]);
    });

    std::array maxDistance = { comp->getProperty<float>("maxDist", NAN) };
    auto m2 = m_audioSourceGroup->createWidget<Drag<float>>("Max Distance", ImGuiDataType_Float, maxDistance, 0.01f, 0.f);
    m2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    m2->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("maxDist", val[0]);
    });

    auto vel = comp->getProperty<Vec3>("velocity", {NAN, NAN, NAN});
    std::array velocity = { vel[0], vel[1], vel[2] };
    auto v2 = m_audioSourceGroup->createWidget<Drag<float, 3>>("Velocity", ImGuiDataType_Float, velocity, 0.01f);
    v2->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    v2->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("velocity", { val[0], val[1], val[2] });
    });

    auto aBlendOp = comp->getProperty<int>("attModel", (int)SoLoud::AudioSource::ATTENUATION_MODELS::LINEAR_DISTANCE);
    auto attenuationModel = m_audioSourceGroup->createWidget<ComboBox>("Attenuation Model", aBlendOp);
    attenuationModel->getOnDataChangedEvent().addListener([this](auto val) {
        storeUndo();
        if(val != -1) getComponent<CompoundComponent>()->setProperty("attModel", val);
    });
    attenuationModel->setEndOfLine(false);
    attenuationModel->addChoice((int)SoLoud::AudioSource::ATTENUATION_MODELS::NO_ATTENUATION, "NO ATTENUATION");
    attenuationModel->addChoice((int)SoLoud::AudioSource::ATTENUATION_MODELS::INVERSE_DISTANCE, "INVERSE DISTANCE");
    attenuationModel->addChoice((int)SoLoud::AudioSource::ATTENUATION_MODELS::LINEAR_DISTANCE, "LINEAR DISTANCE");
    attenuationModel->addChoice((int)SoLoud::AudioSource::ATTENUATION_MODELS::EXPONENTIAL_DISTANCE, "EXPONENTIAL DISTANCE");
    if(aBlendOp == -1) attenuationModel->addChoice(-1, "");
    attenuationModel->setEndOfLine(true);

    std::array attenuationFactor = { comp->getProperty<float>("attFactor", NAN) };
    auto a1 = m_audioSourceGroup->createWidget<Drag<float>>("Attenuation Factor", ImGuiDataType_Float, attenuationFactor, 0.01f, 0.f, 1.f);
    a1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    a1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("attFactor", val[0]);
    });

    std::array dopplerFactor = { comp->getProperty<float>("dopFactor", NAN) };
    auto d1 = m_audioSourceGroup->createWidget<Drag<float>>("Doppler Factor", ImGuiDataType_Float, dopplerFactor, 0.01f, 0.f, 1.f);
    d1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
    });
    d1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("dopFactor", val[0]);
    });
}
NS_IGE_END