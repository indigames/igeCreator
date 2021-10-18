#include "core/scene/components/BoneTransformEditorComponent.h"
#include "core/scene/CompoundComponent.h"

#include "core/layout/Group.h"
#include "core/widgets/Widgets.h"
#include "core/widgets/Label.h"

#include <components/BoneTransform.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>

NS_IGE_BEGIN

BoneTransformEditorComponent ::BoneTransformEditorComponent ()
{
    m_compGroup = nullptr;
}

BoneTransformEditorComponent ::~BoneTransformEditorComponent ()
{
    m_compGroup = nullptr;
}

void BoneTransformEditorComponent ::onInspectorUpdate()
{
    drawComponent();
}

void BoneTransformEditorComponent ::drawComponent()
{
    if (m_compGroup == nullptr)
        m_compGroup = m_group->createWidget<Group>("BoneTransformGroup", false);
    m_compGroup->removeAllWidgets();

    auto comp = getComponent<CompoundComponent>();
    if (comp == nullptr) return;

    if (comp->getComponents().size() == 1) {
        auto boneTransform = std::dynamic_pointer_cast<BoneTransform>(comp->getComponents()[0]);
        if (boneTransform) {
            auto jointObject = boneTransform->getJointObjects();
            if (jointObject.size() > 0)
            {
                auto joinGroup = m_compGroup->createWidget<Group>("Joints", true, false, Group::E_Align::LEFT, false);
                for (const auto& obj : jointObject)
                {
                    auto name = obj.first;
                    joinGroup->createWidget<CheckBox>(name, obj.second != nullptr)->getOnDataChangedEvent().addListener([name, this](bool val) {
                        auto boneTransform = std::dynamic_pointer_cast<BoneTransform>(getComponent<CompoundComponent>()->getComponents()[0]);
                        boneTransform->onJointObjectSelected(name, val);
                        setDirty();
                    });
                }
            }
        }
    }
}
NS_IGE_END