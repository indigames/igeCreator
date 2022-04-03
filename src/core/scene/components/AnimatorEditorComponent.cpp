#include "core/scene/components/AnimatorEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include "core/layout/Group.h"
#include "core/widgets/Widgets.h"
#include "core/widgets/Label.h"
#include "core/FileHandle.h"
#include "core/plugin/DragDropPlugin.h"
#include "core/Editor.h"

#include <components/animation/AnimatorComponent.h>

NS_IGE_BEGIN

AnimatorEditorComponent ::AnimatorEditorComponent ()
{
    m_compGroup = nullptr;
}

AnimatorEditorComponent ::~AnimatorEditorComponent ()
{
    m_compGroup = nullptr;
}

void AnimatorEditorComponent ::onInspectorUpdate()
{
    drawComponent();
}

void AnimatorEditorComponent ::drawComponent()
{
    if (m_compGroup == nullptr)
        m_compGroup = m_group->createWidget<Group>("AnimatorGroup", false);
    m_compGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    if (comp->getComponents().size() == 1) {
        auto animator = std::dynamic_pointer_cast<AnimatorComponent>(comp->getComponents()[0]);
        if (animator) {
            auto txtPath = m_compGroup->createWidget<TextField>("Animator", comp->getProperty<std::string>("path", ""), false, true);
            txtPath->setEndOfLine(false);
            txtPath->getOnDataChangedEvent().addListener([this](const auto& txt) {
                storeUndo();
                getComponent<CompoundComponent>()->setProperty("path", txt);
                setDirty();
            });
            for (const auto& type : GetFileExtensionSuported(E_FileExts::Animator))
            {
                txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](const auto& path) {
                    storeUndo();
                    getComponent<CompoundComponent>()->setProperty("path", GetRelativePath(path));
                    setDirty();
                });
            }
        }
    }
}
NS_IGE_END