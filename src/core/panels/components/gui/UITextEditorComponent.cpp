#include "core/panels/components/gui/UITextEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UIText.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

#include <string>
#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

NS_IGE_BEGIN

UITextEditorComponent::UITextEditorComponent() {
    m_uiTextGroup = nullptr;
}

UITextEditorComponent::~UITextEditorComponent()
{
    if (m_uiTextGroup) {
        m_uiTextGroup->removeAllWidgets();
        m_uiTextGroup->removeAllPlugins();
    }
    m_uiTextGroup = nullptr;
}

bool UITextEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<UIText*>(comp);
}

void UITextEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiTextGroup == nullptr) {
        m_uiTextGroup = m_group->createWidget<Group>("UITextGroup", false);
    }
    drawUIText();

    EditorComponent::redraw();
}

void UITextEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiTextGroup = m_group->createWidget<Group>("UITextGroup", false);

    drawUIText();
}

void UITextEditorComponent::drawUIText()
{
    if (m_uiTextGroup == nullptr)
        return;
    m_uiTextGroup->removeAllWidgets();

    auto uiText = dynamic_cast<UIText*>(getComponent());
    if (uiText == nullptr)
        return;

    auto txtText = m_uiTextGroup->createWidget<TextField>("Text", uiText->getText().c_str());
    txtText->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiText = dynamic_cast<UIText*>(getComponent());
        uiText->setText(txt);
        });

    auto txtFontPath = m_uiTextGroup->createWidget<TextField>("Font", uiText->getFontPath().c_str());
    txtFontPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiText = dynamic_cast<UIText*>(getComponent());
        uiText->setFontPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Font))
    {
        txtFontPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto uiText = dynamic_cast<UIText*>(getComponent());
            uiText->setFontPath(txt);
            dirty();
            });
    }
    
    std::array size = { (int)uiText->getFontSize() };
    m_uiTextGroup->createWidget<Drag<int>>("Size", ImGuiDataType_S32, size, 1, 4, 128)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto uiText = dynamic_cast<UIText*>(getComponent());
        uiText->setFontSize((int)val[0]);
        });

    auto color = uiText->getColor();
    m_uiTextGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto& color) {
        auto uiText = dynamic_cast<UIText*>(getComponent());
        uiText->setColor({ color[0], color[1], color[2], color[3] });
        });
}
NS_IGE_END