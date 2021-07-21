#include "core/scene/components/gui/UITextEditorComponent.h"
#include "core/scene/CompoundComponent.h"

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
        getComponent<CompoundComponent>()->setProperty("text", txt);
    });

    auto txtFontPath = m_uiTextGroup->createWidget<TextField>("Font", comp->getProperty<std::string>("font", ""));
    txtFontPath->getOnDataChangedEvent().addListener([this](auto txt) {
        getComponent<CompoundComponent>()->setProperty("font", txt);
    });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Font)) {
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
}
NS_IGE_END