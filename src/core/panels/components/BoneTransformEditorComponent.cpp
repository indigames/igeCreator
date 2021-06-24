#include "core/panels/components/BoneTransformEditorComponent.h"

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
    if (m_compGroup)
    {
        m_compGroup->removeAllWidgets();
        m_compGroup->removeAllPlugins();
    }
    m_compGroup = nullptr;
}

void BoneTransformEditorComponent ::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_compGroup = m_group->createWidget<Group>("BoneTransformGroup", false);
    drawComponent();
}

void BoneTransformEditorComponent ::drawComponent()
{
    if (m_compGroup == nullptr)
        return;
    m_compGroup->removeAllWidgets();

    auto comp = dynamic_cast<BoneTransform*>(getComponent());
    if (comp == nullptr)
        return;

    auto jointObject = comp->getJointObjects();
    if (jointObject.size() > 0)
    {
        auto joinGroup = m_compGroup->createWidget<Group>("Joints", true, false, Group::E_Align::LEFT, false);
        for (const auto& obj : jointObject)
        {
            auto name = obj.first;
            joinGroup->createWidget<CheckBox>(name, obj.second != nullptr)->getOnDataChangedEvent().addListener([name, this](bool val) {
                auto comp = dynamic_cast<BoneTransform*>(getComponent());
                comp->onJointObjectSelected(name, val);
                redraw();
            });
        }
    }
}
NS_IGE_END