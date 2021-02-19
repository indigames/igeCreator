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
        auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
        spriteComp->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setPath(txt);
            m_bisDirty = true;
            });
    }

    m_spriteCompGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
        if (files.size() > 0)
        {
            auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
            spriteComp->setPath(files[0]);
            m_bisDirty = true;
        }
        });

    std::array size = { spriteComp->getSize().X(), spriteComp->getSize().Y() };
    m_spriteCompGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, size, 1.f, 0.f, 16384.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
        spriteComp->setSize({ val[0], val[1] });
        });

    m_spriteCompGroup->createWidget<CheckBox>("Billboard", spriteComp->isBillboard())->getOnDataChangedEvent().addListener([this](bool val) {
        auto spriteComp = m_targetObject->getComponent<SpriteComponent>();
        spriteComp->setBillboard(val);
        });
}
NS_IGE_END