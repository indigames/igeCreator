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

#include <imgui.h>

namespace ige::creator
{
    ToolBar::ToolBar(const std::string& name, bool enable)
        : Widget(enable), m_name(name)
    {
    }
    
    ToolBar::~ToolBar()
    {
        m_playBtn = nullptr;
        m_stopBtn = nullptr;
    }

    void ToolBar::initialize()
    {
        auto width = ImGui::GetMainViewport()->Size.x / 3.f;
        auto columns = createWidget<Columns<3>>(width);

        auto gizmoGroup = columns->createWidget<Group>("GizmoGroup", false);
        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_translate").c_str())->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
            if (gizmo)
            {
                gizmo->setOperation(gizmo::OPERATION::TRANSLATE);
            }
        });

        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_rotate").c_str())->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
            if (gizmo)
            {
                gizmo->setOperation(gizmo::OPERATION::ROTATE);
            }
        });

        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_scale").c_str())->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
            if (gizmo)
            {
                gizmo->setOperation(gizmo::OPERATION::SCALE);
            }
        });

        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_local").c_str())->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            Editor::getInstance()->toggleLocalGizmo();
            if (Editor::getInstance()->isLocalGizmo())
                ((Button*)widget)->setTextureId(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_local").c_str())->GetTextureHandle());
            else
                ((Button*)widget)->setTextureId(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_global").c_str())->GetTextureHandle());
        });

        gizmoGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_3d").c_str())->GetTextureHandle(), ImVec2(16.f, 16.f), true, false)->getOnClickEvent().addListener([](auto widget) {
            Editor::getInstance()->toggle3DCamera();
            if (Editor::getInstance()->is3DCamera())
                ((Button*)widget)->setTextureId(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_3d").c_str())->GetTextureHandle());
            else
                ((Button*)widget)->setTextureId(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_2d").c_str())->GetTextureHandle());
        });

        auto playGroup = columns->createWidget<Group>("PlayGroup", false, false, Group::E_Align::CENTER);
        m_playBtn = playGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_play").c_str())->GetTextureHandle(), ImVec2(16.f, 16.f), true, false);
        m_playBtn->getOnClickEvent().addListener([this](auto widget) {
            if (!Editor::getCanvas()->getGameScene()->isPlaying() || Editor::getCanvas()->getGameScene()->isPausing()) {
                Editor::getCanvas()->getGameScene()->play();
                if (Editor::getCanvas()->getGameScene()->isPlaying()) {
                    m_stopBtn->setEnable(true);
                    m_playBtn->setTextureId(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_pause").c_str())->GetTextureHandle());
                }
            }
            else {
                Editor::getCanvas()->getGameScene()->pause();
                m_playBtn->setTextureId(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_play").c_str())->GetTextureHandle());
            }
        });

        m_stopBtn = playGroup->createWidget<Button>(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_stop").c_str())->GetTextureHandle(), ImVec2(16.f, 16.f), true, true);
        m_stopBtn->setEnable(false);
        m_stopBtn->getOnClickEvent().addListener([this](auto widget) {
            Editor::getCanvas()->getGameScene()->stop();
            m_stopBtn->setEnable(false);
            m_playBtn->setTextureId(ResourceCreator::Instance().NewTexture(GetEnginePath("icons/btn_play").c_str())->GetTextureHandle());
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
