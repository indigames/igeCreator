#include "core/scene/components/gui/UITextEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "core/Editor.h"

#include <core/layout/Group.h>

#include "components/gui/UIText.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>
#include <string>

NS_IGE_BEGIN

UITextEditorComponent::UITextEditorComponent() {
    m_uiTextGroup = nullptr;
}

UITextEditorComponent::~UITextEditorComponent() {
    m_uiTextGroup = nullptr;
}

void UITextEditorComponent::onInspectorUpdate() {
    drawUIText();
}

void UITextEditorComponent::drawUIText()
{
    if (m_uiTextGroup == nullptr)
        m_uiTextGroup = m_group->createWidget<Group>("UITextGroup", false);;
    m_uiTextGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtText = m_uiTextGroup->createWidget<TextField>("Text", comp->getProperty<std::string>("text", ""));
    txtText->getOnDataChangedEvent().addListener([this](auto txt) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("text", txt);
    });

    auto txtFontPath = m_uiTextGroup->createWidget<TextField>("Font", comp->getProperty<std::string>("font", ""), false, true);
    txtFontPath->getOnDataChangedEvent().addListener([this](const auto& path) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("font", GetRelativePath(path));
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Font)) {
        txtFontPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("font", GetRelativePath(path));
            setDirty();
        });
    }
    
    std::array size = { comp->getProperty<float>("size", NAN) };
    auto s1 = m_uiTextGroup->createWidget<Drag<float>>("Size", ImGuiDataType_S32, size, 1, 4, 300);
    s1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    s1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("size", (int)val[0]);
    });

    m_uiTextGroup->createWidget<Color>("Color", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto& color) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("color", { color[0], color[1], color[2], color[3] });
    });

    auto horizontal = comp->getProperty<int>("alignhorizontal", -1);
    auto horizontalCombo = m_uiTextGroup->createWidget<ComboBox>("AlignHorizontal", horizontal);
    horizontalCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            storeUndo();
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
    auto verticalCombo = m_uiTextGroup->createWidget<ComboBox>("AlignVertical", vertical);
    verticalCombo->getOnDataChangedEvent().addListener([this](auto val) {
        if (val != -1) {
            storeUndo();
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