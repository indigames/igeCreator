#include "core/panels/components/gui/UITextBitmapEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UITextBitmap.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

#include <string>
#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

NS_IGE_BEGIN

UITextBitmapEditorComponent::UITextBitmapEditorComponent() {
    m_uiTextGroup = nullptr;
}

UITextBitmapEditorComponent::~UITextBitmapEditorComponent()
{
    if (m_uiTextGroup) {
        m_uiTextGroup->removeAllWidgets();
        m_uiTextGroup->removeAllPlugins();
    }
    m_uiTextGroup = nullptr;
}

bool UITextBitmapEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<UITextBitmap*>(comp);
}

void UITextBitmapEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiTextGroup == nullptr) {
        m_uiTextGroup = m_group->createWidget<Group>("UITextBitmapGroup", false);
    }
    drawUIText();

    EditorComponent::redraw();
}

void UITextBitmapEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiTextGroup = m_group->createWidget<Group>("UITextBitmapGroup", false);

    drawUIText();
}

void UITextBitmapEditorComponent::drawUIText()
{
    if (m_uiTextGroup == nullptr)
        return;
    m_uiTextGroup->removeAllWidgets();

    auto uiText = m_targetObject->getComponent<UITextBitmap>();
    if (uiText == nullptr)
        return;

    auto txtText = m_uiTextGroup->createWidget<TextField>("Text", uiText->getText().c_str());
    txtText->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiText = m_targetObject->getComponent<UITextBitmap>();
        uiText->setText(txt);
        });

    auto txtFontPath = m_uiTextGroup->createWidget<TextField>("Font", uiText->getFontPath().c_str());
    txtFontPath->setEndOfLine(false);
    txtFontPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiText = m_targetObject->getComponent<UITextBitmap>();
        uiText->setFontPath(txt);
        });


    for (const auto& type : GetFileExtensionSuported(E_FileExts::FontBitmap))
    {
        txtFontPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto uiText = m_targetObject->getComponent<UITextBitmap>();
            uiText->setFontPath(txt);
            dirty();
            });
    }

    auto btnB1 = m_uiTextGroup->createWidget<Button>("...", ImVec2(64.f, 0.f));
    btnB1->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Bitmap (*.pybm)", "*.pybm" }).result();
        if (files.size() > 0)
        {
            auto uiText = m_targetObject->getComponent<UITextBitmap>();
            uiText->setFontPath(files[0]);
            dirty();
        }
        });
    btnB1->setEndOfLine(true);


    
    std::array size = { (int)uiText->getFontSize() };
    m_uiTextGroup->createWidget<Drag<int>>("Size", ImGuiDataType_S32, size, 1, 4, 128)->getOnDataChangedEvent().addListener([this](auto& val) {
        auto uiText = m_targetObject->getComponent<UITextBitmap>();
        uiText->setFontSize((int)val[0]);
        });

    auto color = uiText->getColor();
    m_uiTextGroup->createWidget<Color>("Color", color)->getOnDataChangedEvent().addListener([this](auto& color) {
        auto uiText = m_targetObject->getComponent<UITextBitmap>();
        uiText->setColor({ color[0], color[1], color[2], color[3] });
        });
}
NS_IGE_END