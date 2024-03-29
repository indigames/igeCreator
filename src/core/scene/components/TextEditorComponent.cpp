#include "core/scene/components/TextEditorComponent.h"
#include "core/scene/CompoundComponent.h"
#include "core/Editor.h"

#include "core/layout/Group.h"
#include "core/widgets/Widgets.h"
#include "core/FileHandle.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/dialog/OpenFileDialog.h"

#include <components/SpriteComponent.h>
using namespace ige::scene;

NS_IGE_BEGIN

TextEditorComponent::TextEditorComponent() {
    m_textCompGroup = nullptr;
}

TextEditorComponent::~TextEditorComponent()
{
    m_textCompGroup = nullptr;
}

void TextEditorComponent::onInspectorUpdate()
{   
    drawComponent();
}

void TextEditorComponent::drawComponent()
{
    if (m_textCompGroup == nullptr) {
        m_textCompGroup = m_group->createWidget<Group>("TextGroup", false);
    }
    m_textCompGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    auto txtText = m_textCompGroup->createWidget<TextField>("Text", comp->getProperty<std::string>("text", ""));
    txtText->getOnDataChangedEvent().addListener([this](auto txt) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("text", txt);
        });

    auto txtFontPath = m_textCompGroup->createWidget<TextField>("Font", comp->getProperty<std::string>("font", ""), false, true);
    txtFontPath->getOnDataChangedEvent().addListener([this](auto txt) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("font", txt);
        });
    for (const auto& type : GetFileExtensionSuported(E_FileExts::Font)) {
        txtFontPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
            storeUndo();
            getComponent<CompoundComponent>()->setProperty("font", GetRelativePath(path));
            setDirty();
            });
    }

    std::array size = { comp->getProperty<float>("size", NAN) };
    auto s1 = m_textCompGroup->createWidget<Drag<float>>("Size", ImGuiDataType_S32, size, 1, 4, 300);
    s1->getOnDataBeginChangedEvent().addListener([this](auto val) {
        storeUndo();
        });
    s1->getOnDataChangedEvent().addListener([this](auto& val) {
        getComponent<CompoundComponent>()->setProperty("size", (int)val[0]);
        });

    m_textCompGroup->createWidget<Color>("Color", comp->getProperty<Vec4>("color", { NAN, NAN, NAN, NAN }))->getOnDataChangedEvent().addListener([this](auto& color) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("color", { color[0], color[1], color[2], color[3] });
        });

    m_textCompGroup->createWidget<CheckBox>("Billboard", comp->getProperty<bool>("billboard", false))->getOnDataChangedEvent().addListener([this](bool val) {
        storeUndo();
        getComponent<CompoundComponent>()->setProperty("billboard", val);
    });
}
NS_IGE_END