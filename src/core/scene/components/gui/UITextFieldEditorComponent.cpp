#include "core/scene/components/gui/UITextFieldEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "core/Editor.h"

#include <core/layout/Group.h>

#include "components/gui/UITextField.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

#include <string>

NS_IGE_BEGIN

UITextFieldEditorComponent::UITextFieldEditorComponent() {
    m_uiTextFieldGroup = nullptr;
}

UITextFieldEditorComponent::~UITextFieldEditorComponent() {
    m_uiTextFieldGroup = nullptr;
}

void UITextFieldEditorComponent::onInspectorUpdate() {
    drawUITextField();
}

void UITextFieldEditorComponent::drawUITextField() {
    if (m_uiTextFieldGroup == nullptr)
        m_uiTextFieldGroup = m_group->createWidget<Group>("UITextFieldGroup", false);;
    m_uiTextFieldGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtText = m_uiTextFieldGroup->createWidget<TextField>("Text", comp->getProperty<std::string>("text", ""));
    txtText->getOnDataChangedEvent().addListener([this](auto txt) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("text", txt);
    });

    auto txtFontPath = m_uiTextFieldGroup->createWidget<TextField>("Font", comp->getProperty<std::string>("font", ""), false, true);
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
    auto s1 = m_uiTextFieldGroup->createWidget<Drag<float>>("Size", ImGuiDataType_S32, size, 1, 4, 300);
    s1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    s1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("size", (int)val[0]);
    });

    m_uiTextFieldGroup->createWidget<Color>("Color", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto& color) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("color", { color[0], color[1], color[2], color[3] });
    });
}
NS_IGE_END
