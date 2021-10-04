#include "core/scene/components/SpriteEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include "core/layout/Group.h"
#include "core/widgets/Widgets.h"
#include "core/FileHandle.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"

#include <Components/SpriteComponent.h>
using namespace ige::scene;

NS_IGE_BEGIN

SpriteEditorComponent::SpriteEditorComponent() {
    m_spriteCompGroup = nullptr;
}

SpriteEditorComponent::~SpriteEditorComponent()
{
    m_spriteCompGroup = nullptr;
}

void SpriteEditorComponent::onInspectorUpdate()
{   
    drawSpriteComponent();
}

void SpriteEditorComponent::drawSpriteComponent()
{
    if (m_spriteCompGroup == nullptr) {
        m_spriteCompGroup = m_group->createWidget<Group>("SpriteGroup", false);
    }
    m_spriteCompGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtPath = m_spriteCompGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""), false, true);
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](const auto& txt) {
        getComponent<CompoundComponent>()->setProperty("path", txt);
    });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& txt) {
            getComponent<CompoundComponent>()->setProperty("path", txt);
            setDirty();
        });
    }

    m_spriteCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
        if (files.size() > 0) {
            getComponent<CompoundComponent>()->setProperty("path", files[0]);
            setDirty();
        }
    });

    auto size = comp->getProperty<Vec2>("size", { NAN, NAN });
    std::array sizeArr = { size.X(), size.Y() };
    m_spriteCompGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, sizeArr, 1.f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("size", { val[0], val[1] });
    });

    auto color = comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN });
    m_spriteCompGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
    });

    m_spriteCompGroup->createWidget<CheckBox>("Billboard", comp->getProperty<bool>("billboard", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("billboard", val);
    });

    auto spriteType = comp->getProperty<int>("spritetype", -1);
    auto spriteTypeCombo = m_spriteCompGroup->createWidget<ComboBox>("Sprite Type", spriteType);
    spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            getComponent<CompoundComponent>()->setProperty("spritetype", val);
            setDirty();
        }
    });
    spriteTypeCombo->setEndOfLine(false);
    spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    if (spriteType == -1) spriteTypeCombo->addChoice(-1, "");
    spriteTypeCombo->setEndOfLine(true);

    if (spriteType == (int)SpriteType::Sliced) {
        auto border = comp->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
        std::array borderLeft = { border[0] };
        m_spriteCompGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto comp = getComponent<CompoundComponent>();
            auto border = comp->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border[0] = val[0];
            comp->setProperty("border", border);
        });
        std::array borderRight = { border[1]};
        m_spriteCompGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto comp = getComponent<CompoundComponent>();
            auto border = comp->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border[1] = val[0];
            comp->setProperty("border", border);
        });
        std::array borderTop = { border[2]};
        m_spriteCompGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto comp = getComponent<CompoundComponent>();
            auto border = comp->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border[2] = val[0];
            comp->setProperty("border", border);
        });
        std::array borderBottom = { border[3]};
        m_spriteCompGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto comp = getComponent<CompoundComponent>();
            auto border = comp->getProperty<Vec4>("border", { NAN, NAN, NAN, NAN });
            border[3] = val[0];
            comp->setProperty("border", border);
        });
    }
    else {
        auto tiling = comp->getProperty<Vec2>("tiling", { NAN, NAN });
        std::array tilingArr = { tiling.X(), tiling.Y() };
        m_spriteCompGroup->createWidget<Drag<float, 2>>("Tiling", ImGuiDataType_Float, tilingArr, 0.01f, -16384.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("tiling", { val[0], val[1] });
        });
        auto offset = comp->getProperty<Vec2>("offset", { NAN, NAN });
        std::array offsetArr = { offset.X(), offset.Y() };
        m_spriteCompGroup->createWidget<Drag<float, 2>>("Offset", ImGuiDataType_Float, offsetArr, 0.01f, -16384.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("offset", { val[0], val[1] });
        });

        auto wrapMode = comp->getProperty<int>("wrapmode", -1);
        auto compCombo = m_spriteCompGroup->createWidget<ComboBox>("WrapMode", wrapMode);
        compCombo->getOnDataChangedEvent().addListener([this](auto val) {
            if (val != -1) {
                getComponent<CompoundComponent>()->setProperty("wrapmode", val);
                setDirty();
            }
        });
        compCombo->setEndOfLine(false);
        compCombo->addChoice((int)SamplerState::WrapMode::WRAP, "Wrap");
        compCombo->addChoice((int)SamplerState::WrapMode::MIRROR, "Mirror");
        compCombo->addChoice((int)SamplerState::WrapMode::CLAMP, "Clamp");
        compCombo->addChoice((int)SamplerState::WrapMode::BORDER, "Border");
        if(wrapMode == -1) compCombo->addChoice(-1, "");
        compCombo->setEndOfLine(true);
    }

    m_spriteCompGroup->createWidget<CheckBox>("Alpha Blend", comp->getProperty<bool>("aBlend", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("aBlend", val);
        setDirty();
    });

    if (comp->getProperty<bool>("aBlend", false))
    {
        auto blendOp = comp->getProperty<int>("aBlendOp", -1);
        auto alphaCombo = m_spriteCompGroup->createWidget<ComboBox>("Blend OP", blendOp);
        alphaCombo->getOnDataChangedEvent().addListener([this](auto val) {
            if (val != -1) {
                getComponent<CompoundComponent>()->setProperty("aBlendOp", val);
                setDirty();
            }
        });
        alphaCombo->setEndOfLine(false);
        alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::COL, "COL");
        alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::ADD, "ADD");
        alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::SUB, "SUB");
        alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::MUL, "MUL");
        if(blendOp == -1) alphaCombo->addChoice(-1, "");
        alphaCombo->setEndOfLine(true);
    }
}
NS_IGE_END