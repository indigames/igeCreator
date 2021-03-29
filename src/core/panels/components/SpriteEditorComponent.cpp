#include "core/panels/components/SpriteEditorComponent.h"

#include <core/layout/Group.h>

#include "components/SpriteComponent.h"

#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

SpriteEditorComponent::SpriteEditorComponent() {
    m_spriteCompGroup = nullptr;
}

SpriteEditorComponent::~SpriteEditorComponent()
{
    if (m_spriteCompGroup) {
        m_spriteCompGroup->removeAllWidgets();
        m_spriteCompGroup->removeAllPlugins();
    }
    m_spriteCompGroup = nullptr;
}

bool SpriteEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<SpriteComponent*>(comp);
}

void SpriteEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_spriteCompGroup == nullptr) {
        m_spriteCompGroup = m_group->createWidget<Group>("SpriteGroup", false);
    }
    drawSpriteComponent();

    EditorComponent::redraw();
}

void SpriteEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_spriteCompGroup = m_group->createWidget<Group>("SpriteGroup", false);

    drawSpriteComponent();
}

void SpriteEditorComponent::drawSpriteComponent()
{
    if (m_spriteCompGroup == nullptr)
        return;
    m_spriteCompGroup->removeAllWidgets();

    auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
    if (spriteComp == nullptr)
        return;

    auto txtPath = m_spriteCompGroup->createWidget<TextField>("Path", spriteComp->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
        spriteComp->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setPath(txt);
            dirty();
            });
    }

    m_spriteCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
        if (files.size() > 0)
        {
            auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
            spriteComp->setPath(files[0]);
            dirty();
        }
        });

    std::array size = { spriteComp->getSize().X(), spriteComp->getSize().Y() };
    m_spriteCompGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, size, 1.f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
        spriteComp->setSize({ val[0], val[1] });
        });

    auto color = spriteComp->getColor();
    m_spriteCompGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto val) {
        auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
        spriteComp->setColor(val[0], val[1], val[2], val[3]);
        });

    m_spriteCompGroup->createWidget<CheckBox>("Billboard", spriteComp->isBillboard())->getOnDataChangedEvent().addListener([this](bool val) {
        auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
        spriteComp->setBillboard(val);
        });

    std::array tiling = { spriteComp->getTiling().X(), spriteComp->getTiling().Y() };
    m_spriteCompGroup->createWidget<Drag<float, 2>>("Tiling", ImGuiDataType_Float, tiling, 0.01f, -16384.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
        spriteComp->setTiling({ val[0], val[1] });
        });

    std::array offset = { spriteComp->getOffset().X(), spriteComp->getOffset().Y() };
    m_spriteCompGroup->createWidget<Drag<float, 2>>("Offset", ImGuiDataType_Float, offset, 0.01f, -16384.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
        spriteComp->setOffset({ val[0], val[1] });
        });
        
    auto m_compCombo = m_spriteCompGroup->createWidget<ComboBox>((int)spriteComp->getWrapMode());
    m_compCombo->getOnDataChangedEvent().addListener([this](auto val) {
        auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
        spriteComp->setWrapMode(val);
        dirty();
        });
    m_compCombo->setEndOfLine(false);
    m_compCombo->addChoice((int)SamplerState::WrapMode::WRAP, "Wrap");
    m_compCombo->addChoice((int)SamplerState::WrapMode::MIRROR, "Mirror");
    m_compCombo->addChoice((int)SamplerState::WrapMode::CLAMP, "Clamp");
    m_compCombo->addChoice((int)SamplerState::WrapMode::BORDER, "Border");
    m_compCombo->setEndOfLine(true);
    
    m_spriteCompGroup->createWidget<CheckBox>("Alpha Blend", spriteComp->isAlphaBlendingEnable())->getOnDataChangedEvent().addListener([this](bool val) {
        auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
        spriteComp->setAlphaBlendingEnable(val);
        dirty();
    });

    if (spriteComp->isAlphaBlendingEnable())
    {
        auto m_alphaCombo = m_spriteCompGroup->createWidget<ComboBox>((int)spriteComp->getAlphaBlendingOp());
        m_alphaCombo->getOnDataChangedEvent().addListener([this](auto val) {
            auto spriteComp = dynamic_cast<SpriteComponent*>(m_component);
            spriteComp->setAlphaBlendingOp(val);
            dirty();
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