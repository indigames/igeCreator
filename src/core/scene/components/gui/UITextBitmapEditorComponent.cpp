#include "core/scene/components/gui/UITextBitmapEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UITextBitmap.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>
#include <string>


NS_IGE_BEGIN

UITextBitmapEditorComponent::UITextBitmapEditorComponent() {
    m_uiTextGroup = nullptr;
}

UITextBitmapEditorComponent::~UITextBitmapEditorComponent() {
    if (m_uiTextGroup) {
        m_uiTextGroup->removeAllWidgets();
        m_uiTextGroup->removeAllPlugins();
    }
    m_uiTextGroup = nullptr;
}

void UITextBitmapEditorComponent::onInspectorUpdate() {
    drawUIText();
}

void UITextBitmapEditorComponent::drawUIText()
{
    if (m_uiTextGroup == nullptr)
        m_uiTextGroup = m_group->createWidget<Group>("UITextBitmapGroup", false);;
    m_uiTextGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtText = m_uiTextGroup->createWidget<TextField>("Text", comp->getProperty<std::string>("text", ""));
    txtText->getOnDataChangedEvent().addListener([this](auto txt) {
        getComponent<CompoundComponent>()->setProperty("text", txt);
    });

    auto txtFontPath = m_uiTextGroup->createWidget<TextField>("Font", comp->getProperty<std::string>("font", ""), false, true);
    txtFontPath->getOnDataChangedEvent().addListener([this](auto txt) {
        getComponent<CompoundComponent>()->setProperty("font", txt);
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::FontBitmap)) {
        txtFontPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            getComponent<CompoundComponent>()->setProperty("font", txt);
            setDirty();
        });
    }

    std::array size = { comp->getProperty<float>("size", NAN) };
    m_uiTextGroup->createWidget<Drag<float>>("Size", ImGuiDataType_S32, size, 1, 4, 1024)->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("size", (int)val[0]);
    });

    m_uiTextGroup->createWidget<Color>("Color", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto& color) {
        getComponent<CompoundComponent>()->setProperty("color", { color[0], color[1], color[2], color[3] });
    });

    auto horizontal = comp->getProperty<int>("alignhorizontal", -1);
    auto horizontalCombo = m_uiTextGroup->createWidget<ComboBox>("AlignHorizontal", horizontal);
    horizontalCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            getComponent<CompoundComponent>()->setProperty("alignhorizontal", val);
            setDirty();
        }
        });
    horizontalCombo->setEndOfLine(false);
    horizontalCombo->addChoice(0, "Left");
    horizontalCombo->addChoice(1, "Center");
    horizontalCombo->addChoice(2, "Right");
    horizontalCombo->setEndOfLine(true);

    auto vertical = comp->getProperty<int>("alignvertical", -1);
    auto verticalCombo = m_uiTextGroup->createWidget<ComboBox>("Text Align Horizontal", vertical);
    verticalCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            getComponent<CompoundComponent>()->setProperty("alignvertical", val);
            setDirty();
        }
        });
    verticalCombo->setEndOfLine(false);
    verticalCombo->addChoice(0, "Top");
    verticalCombo->addChoice(1, "Center");
    verticalCombo->addChoice(2, "Bottom");
    verticalCombo->setEndOfLine(true);
}
NS_IGE_END