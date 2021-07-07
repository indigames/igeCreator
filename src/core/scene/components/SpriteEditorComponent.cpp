#include "core/scene/components/SpriteEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include "core/layout/Group.h"
#include "core/widgets/Widgets.h"
#include "core/FileHandle.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"

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

    auto txtPath = m_spriteCompGroup->createWidget<TextField>("Path", comp->getProperty<std::string>("path", ""));
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

    auto size = comp->getProperty<Vec2>("size", {0, 0});
    std::array sizeArr = { size.X(), size.Y() };
    m_spriteCompGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, sizeArr, 1.f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("size", { val[0], val[1] });
    });

    auto color = comp->getProperty<Vec4>("color", { 0, 0, 0, 0});
    m_spriteCompGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("color", { val[0], val[1], val[2], val[3] });
    });

    m_spriteCompGroup->createWidget<CheckBox>("Billboard", comp->getProperty<bool>("billboard", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("billboard", val);
    });

    auto spriteType = comp->getProperty<int>("spritetype", 0);
    auto m_spriteTypeCombo = m_spriteCompGroup->createWidget<ComboBox>("", (int)spriteType);
    m_spriteTypeCombo->getOnDataChangedEvent().addListener([this](auto val) {
        getComponent<CompoundComponent>()->setProperty("spritetype", val);
        setDirty();
    });
    m_spriteTypeCombo->setEndOfLine(false);
    m_spriteTypeCombo->addChoice((int)SpriteType::Simple, "Simple");
    m_spriteTypeCombo->addChoice((int)SpriteType::Sliced, "Sliced");
    m_spriteTypeCombo->setEndOfLine(true);

    if (spriteType == (int)SpriteType::Sliced) {
        auto border = comp->getProperty<Vec4>("border", { 0, 0, 0, 0 });
        std::array borderLeft = { border[0] };
        m_spriteCompGroup->createWidget<Drag<float, 1>>("Border Left", ImGuiDataType_Float, borderLeft, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto comp = getComponent<CompoundComponent>();
            auto border = comp->getProperty<Vec4>("border", { 0, 0, 0, 0 });
            border[0] = val[0];
            comp->setProperty("border", border);
        });
        std::array borderRight = { border[1]};
        m_spriteCompGroup->createWidget<Drag<float, 1>>("Border Right", ImGuiDataType_Float, borderRight, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto comp = getComponent<CompoundComponent>();
            auto border = comp->getProperty<Vec4>("border", { 0, 0, 0, 0 });
            border[1] = val[0];
            comp->setProperty("border", border);
        });
        std::array borderTop = { border[2]};
        m_spriteCompGroup->createWidget<Drag<float, 1>>("Border Top", ImGuiDataType_Float, borderTop, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto comp = getComponent<CompoundComponent>();
            auto border = comp->getProperty<Vec4>("border", { 0, 0, 0, 0 });
            border[2] = val[0];
            comp->setProperty("border", border);
        });
        std::array borderBottom = { border[3]};
        m_spriteCompGroup->createWidget<Drag<float, 1>>("Border Bottom", ImGuiDataType_Float, borderBottom, 1.0f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto comp = getComponent<CompoundComponent>();
            auto border = comp->getProperty<Vec4>("border", { 0, 0, 0, 0 });
            border[3] = val[0];
            comp->setProperty("border", border);
        });
    }
    else {
        auto tiling = comp->getProperty<Vec2>("tiling", {0, 0});
        std::array tilingArr = { tiling.X(), tiling.Y() };
        m_spriteCompGroup->createWidget<Drag<float, 2>>("Tiling", ImGuiDataType_Float, tilingArr, 0.01f, -16384.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("tiling", { val[0], val[1] });
        });
        auto offset = comp->getProperty<Vec2>("offset", { 0, 0 });
        std::array offsetArr = { offset.X(), offset.Y() };
        m_spriteCompGroup->createWidget<Drag<float, 2>>("Offset", ImGuiDataType_Float, offsetArr, 0.01f, -16384.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("offset", { val[0], val[1] });
        });

        auto m_compCombo = m_spriteCompGroup->createWidget<ComboBox>("WrapMode", comp->getProperty<int>("wrapmode", 0));
        m_compCombo->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("wrapmode", val);
            setDirty();
        });
        m_compCombo->setEndOfLine(false);
        m_compCombo->addChoice((int)SamplerState::WrapMode::WRAP, "Wrap");
        m_compCombo->addChoice((int)SamplerState::WrapMode::MIRROR, "Mirror");
        m_compCombo->addChoice((int)SamplerState::WrapMode::CLAMP, "Clamp");
        m_compCombo->addChoice((int)SamplerState::WrapMode::BORDER, "Border");
        m_compCombo->setEndOfLine(true);
    }

    m_spriteCompGroup->createWidget<CheckBox>("Alpha Blend", comp->getProperty<bool>("aBlend", false))->getOnDataChangedEvent().addListener([this](bool val) {
        getComponent<CompoundComponent>()->setProperty("aBlend", val);
        setDirty();
    });

    if (comp->getProperty<bool>("aBlend", false))
    {
        auto m_alphaCombo = m_spriteCompGroup->createWidget<ComboBox>("Blend OP", comp->getProperty<int>("aBlendOp", 0));
        m_alphaCombo->getOnDataChangedEvent().addListener([this](auto val) {
            getComponent<CompoundComponent>()->setProperty("aBlendOp", val);
            setDirty();
        });
        m_alphaCombo->setEndOfLine(false);
        m_alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::COL, "COL");
        m_alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::ADD, "ADD");
        m_alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::SUB, "SUB");
        m_alphaCombo->addChoice((int)ShaderDescriptor::AlphaBlendOP::MUL, "MUL");
        m_alphaCombo->setEndOfLine(true);
    }
}
NS_IGE_END