#include "core/panels/components/gui/UIImageEditorComponent.h"

#include <core/layout/Group.h>

#include "components/gui/UIImage.h"
#include "core/widgets/Widgets.h"
#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

NS_IGE_BEGIN

UIImageEditorComponent::UIImageEditorComponent() {
    m_uiImageGroup = nullptr;
}

UIImageEditorComponent::~UIImageEditorComponent()
{
    if (m_uiImageGroup) {
        m_uiImageGroup->removeAllWidgets();
        m_uiImageGroup->removeAllPlugins();
    }
    m_uiImageGroup = nullptr;
}

bool UIImageEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<UIImage*>(comp);
}

void UIImageEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_uiImageGroup == nullptr) {
        m_uiImageGroup = m_group->createWidget<Group>("UIImageGroup", false);
    }
    drawUIImage();

    EditorComponent::redraw();
}

void UIImageEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_uiImageGroup = m_group->createWidget<Group>("UIImageGroup", false);

    drawUIImage();
}

void UIImageEditorComponent::drawUIImage()
{
    if (m_uiImageGroup == nullptr)
        return;
    m_uiImageGroup->removeAllWidgets();

    auto uiImage = m_targetObject->getComponent<UIImage>();
    if (uiImage == nullptr)
        return;

    auto txtPath = m_uiImageGroup->createWidget<TextField>("Path", uiImage->getPath());
    txtPath->setEndOfLine(false);
    txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
        auto uiImage = m_targetObject->getComponent<UIImage>();
        uiImage->setPath(txt);
        });

    for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
    {
        txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
            auto uiImage = m_targetObject->getComponent<UIImage>();
            uiImage->setPath(txt);
            dirty();
            });
    }

    m_uiImageGroup->createWidget<Button>("Browse", ImVec2(64.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
        auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
        if (files.size() > 0)
        {
            auto uiImage = m_targetObject->getComponent<UIImage>();
            uiImage->setPath(files[0]);
            dirty();
        }
        });
}
NS_IGE_END