#include <imgui.h>

#include "core/toolbar/ToolBar.h"

#include "core/Widget.h"
#include "core/widgets/Button.h"
#include "core/widgets/TextField.h"
#include "core/widgets/Label.h"
#include "core/widgets/Slider.h"
#include "core/widgets/Separator.h"

#include "core/layout/Columns.h"
#include "core/layout/Group.h"

#include "utils/PyxieHeaders.h"
#include "core/Editor.h"
#include "core/panels/EditorScene.h"
#include "core/panels/GameScene.h"
#include "core/Canvas.h"

namespace ige::creator
{
    ToolBar::ToolBar(const std::string& name, bool enable)
        : Widget(enable), m_name(name)
    {
    }
    
    ToolBar::~ToolBar()
    {
    }

    void ToolBar::initialize()
    {
        auto width = ImGui::GetMainViewport()->Size.x / 3.f;
        auto columns = createWidget<Columns<3>>(width);

        auto gizmoGroup = columns->createWidget<Group>("GizmoGroup", false);
        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/btn_translate")->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
            if (gizmo)
            {
                gizmo->setOperation(gizmo::OPERATION::TRANSLATE);
            }
        });

        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/btn_rotate")->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
            if (gizmo)
            {
                gizmo->setOperation(gizmo::OPERATION::ROTATE);
            }
        });

        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/btn_scale")->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
            if (gizmo)
            {
                gizmo->setOperation(gizmo::OPERATION::SCALE);
            }
        });

        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/btn_local")->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            Editor::getInstance()->toggleLocalGizmo();
            if (Editor::getInstance()->isLocalGizmo())
                (Button*)(widget)->setTextureId(ResourceCreator::Instance().NewTexture("icon/btn_local")->GetTextureHandle());
            else
                (Button*)(widget)->setTextureId(ResourceCreator::Instance().NewTexture("icon/btn_global")->GetTextureHandle());
        });

        auto playGroup = columns->createWidget<Group>("PlayGroup", false, false, Group::E_Align::CENTER);
        playGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/btn_play")->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget){
            Editor::getCanvas()->getGameScene()->play();
        });        

        playGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/btn_pause")->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget){
            Editor::getCanvas()->getGameScene()->pause();
        });        

        playGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture("icon/btn_stop")->GetTextureHandle(), ImVec2(16.f, 16.f), true, true)->getOnClickEvent().addListener([](auto widget){
            Editor::getCanvas()->getGameScene()->stop();
        });

        auto serviceGroup = columns->createWidget<Group>("ServiceGroup", false, false, Group::E_Align::RIGHT);
        serviceGroup->createWidget<Label>("Service (coming soon...)");
    }

    void ToolBar::_drawImpl()
    {
        static bool initialized = false;
        if(!initialized)
        {
            initialize();
            initialized = true;
        }

        drawWidgets();
    }    
}
