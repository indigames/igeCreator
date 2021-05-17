#include "core/EditorUtils.h"
#include "core/gizmo/Gizmo.h"
#include "core/Editor.h"
#include "core/Canvas.h"

NS_IGE_BEGIN

void EditorUtils::Tools_View() 
{
    bool focused = Editor::getCanvas()->getHierarchy()->isFocused() || Editor::getCanvas()->getEditorScene()->isFocused();
    auto targets = Editor::getCurrentScene()->getTargets();
    if (!targets.empty() && focused) {
        auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
        if (gizmo) gizmo->setVisible(false);
    }
}

void EditorUtils::Tools_Move()
{
    bool focused = Editor::getCanvas()->getHierarchy()->isFocused() || Editor::getCanvas()->getEditorScene()->isFocused();
    auto targets = Editor::getCurrentScene()->getTargets();
    if (!targets.empty() && focused)
    {
        auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
        if (gizmo)
        {
            gizmo->setVisible(true);
            gizmo->setOperation(gizmo::OPERATION::TRANSLATE);
        }
    }
}

void EditorUtils::Tools_Rotate() 
{
    bool focused = Editor::getCanvas()->getHierarchy()->isFocused() || Editor::getCanvas()->getEditorScene()->isFocused();
    auto targets = Editor::getCurrentScene()->getTargets();
    if (!targets.empty() && focused)
    {
        auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
        if (gizmo)
        {
            gizmo->setVisible(true);
            gizmo->setOperation(gizmo::OPERATION::ROTATE);
        }
    }
}

void EditorUtils::Tools_Scale()
{
    bool focused = Editor::getCanvas()->getHierarchy()->isFocused() || Editor::getCanvas()->getEditorScene()->isFocused();
    auto targets = Editor::getCurrentScene()->getTargets();
    if (!targets.empty() && focused)
    {
        auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
        if (gizmo)
        {
            gizmo->setVisible(true);
            gizmo->setOperation(gizmo::OPERATION::SCALE);
        }
    }
}

void EditorUtils::Tools_TogglePivotPosition() 
{

}

void EditorUtils::Tools_TogglePivotOrientation()
{
    Editor::getInstance()->toggleLocalGizmo();
}

NS_IGE_END