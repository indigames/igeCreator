#include "core/EditorUtils.h"
#include "core/gizmo/Gizmo.h"
#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/Hierarchy.h"
#include "core/panels/EditorScene.h"
#include "core/scene/TargetObject.h"

#ifdef WIN32
#  include <window.h>
#endif

NS_IGE_BEGIN

void EditorUtils::Tools_View() 
{
    bool focused = Editor::getCanvas()->getHierarchy()->isFocused() || Editor::getCanvas()->getEditorScene()->isFocused();
    auto target = Editor::getInstance()->getTarget();
    if (target && !target->empty() && focused) {
        auto gizmo = Editor::getCanvas()->getEditorScene()->getGizmo();
        if (gizmo) gizmo->setVisible(false);
    }
}

void EditorUtils::Tools_Move()
{
    bool focused = Editor::getCanvas()->getHierarchy()->isFocused() || Editor::getCanvas()->getEditorScene()->isFocused();
    auto target = Editor::getInstance()->getTarget();
    if (target && !target->empty() && focused)
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
    auto target = Editor::getInstance()->getTarget();
    if (target && !target->empty() && focused)
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
    auto target = Editor::getInstance()->getTarget();
    if (target && !target->empty() && focused)
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

#ifdef _WIN32
void EditorUtils::openExplorer(const std::string &path) {
    auto fsPath = fs::path(path);
    ShellExecute(NULL, "explore", fs::is_directory(fsPath) ? path.c_str() : fsPath.parent_path().string().c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void EditorUtils::openFile(const std::string &path) {
    if(fs::is_directory(fs::path(path))) {
        ShellExecute(NULL, "explore", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
    else {
        ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_RESTORE);
    }
}
#endif

NS_IGE_END
