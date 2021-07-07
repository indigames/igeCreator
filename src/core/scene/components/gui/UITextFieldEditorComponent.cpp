#include "core/scene/components/gui/UITextFieldEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UITextField.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

#include <string>
#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

NS_IGE_BEGIN

UITextFieldEditorComponent::UITextFieldEditorComponent() {
    m_uiTextFieldGroup = nullptr;
}

UITextFieldEditorComponent::~UITextFieldEditorComponent()
{
    m_uiTextFieldGroup = nullptr;
}

void UITextFieldEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiTextFieldGroup == nullptr) {
        m_uiTextFieldGroup = m_group->createWidget<Group>("UITextFieldGroup", false);
    }
    drawUITextField();

    EditorComponent::redraw();
}

void UITextFieldEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiTextFieldGroup = m_group->createWidget<Group>("UITextFieldGroup", false);

    drawUITextField();
}

void UITextFieldEditorComponent::drawUITextField()
{
    if (m_uiTextFieldGroup == nullptr)
        return;
    m_uiTextFieldGroup->removeAllWidgets();

    auto uiText = getComponent<UITextField>();
    if (uiText == nullptr)
        return;

    auto txtText = m_uiTextFieldGroup->createWidget<TextField>("Text", uiText->getText().c_str());
    txtText->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiText = getComponent<UITextField>();
        uiText->setText(txt);
        });

    auto txtFontPath = m_uiTextFieldGroup->createWidget<TextField>("Font", uiText->getFontPath().c_str());
    txtFontPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiText = getComponent<UITextField>();
        uiText->setFontPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Font))
    {
        txtFontPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto uiText = getComponent<UITextField>();
            uiText->setFontPath(txt);
            setDirty();
            });
    }

    std::array size = { (int)uiText->getFontSize() };
    m_uiTextFieldGroup->createWidget<Drag<int>>("Size", ImGuiDataType_S32, size, 1, 4, 128)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto uiText = getComponent<UITextField>();
        uiText->setFontSize((int)val[0]);
        });

    auto color = uiText->getColor();
    m_uiTextFieldGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto& color) {
        auto uiText = getComponent<UITextField>();
        uiText->setColor({ color[0], color[1], color[2], color[3] });
        });
}
NS_IGE_END