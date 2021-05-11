#include "core/shortcut/ShortcutDictionary.h"
#include "core/Editor.h"
#include "core/EditorUtils.h"
#include "core/shortcut/ShortcutController.h"
#include "core/shortcut/IShortcut.h"
#include <functional>
NS_IGE_BEGIN

const char* ShortcutDictionary::TOOL_VIEW = "Tools/View";
const char* ShortcutDictionary::TOOL_MOVE = "Tools/Move";
const char* ShortcutDictionary::TOOL_ROTATE = "Tools/Rotate";
const char* ShortcutDictionary::TOOL_SCALE = "Tools/Scale";
const char* ShortcutDictionary::TOOL_PIVOT_POSITION = "Tools/Toggle Pivot Position";
const char* ShortcutDictionary::TOOL_PIVOT_ORIENTATION = "Tools/Toggle Pivot Orientation";

const char* ShortcutDictionary::EDIT_SCENE_OBJECT_SELECTED = "Edit/Scene Object Selected";
const char* ShortcutDictionary::DELETE_SCENE_OBJECT_SELECTED = "Edit/Delete Object Selected";
const char* ShortcutDictionary::COPY_SCENE_OBJECT_SELECTED = "Edit/Copy Object Selected";
const char* ShortcutDictionary::PASTE_SCENE_OBJECT_SELECTED = "Edit/Paste Object Selected";
const char* ShortcutDictionary::SAVE_SCENE_SELECTED = "Edit/Save Scene";


void ShortcutDictionary::initShortcuts() {
	initDefaultShortcutCommand();
	assignDefaultShortcut();
	ShortcutRegistration::getInstance()->registerShortcuts();
}

//! Default Shortcut Store
void ShortcutDictionary::initDefaultShortcutCommand() {
	ASSIGN_COMMAND_TO_DICT(TOOL_VIEW, EditorUtils::Tools_View);
	ASSIGN_COMMAND_TO_DICT(TOOL_MOVE, EditorUtils::Tools_Move);
	ASSIGN_COMMAND_TO_DICT(TOOL_ROTATE, EditorUtils::Tools_Rotate);
	ASSIGN_COMMAND_TO_DICT(TOOL_SCALE, EditorUtils::Tools_Scale);

	ASSIGN_COMMAND_TO_DICT(TOOL_PIVOT_ORIENTATION, EditorUtils::Tools_TogglePivotOrientation);
}

void ShortcutDictionary::assignDefaultShortcut() {
	ASSIGN_KEY_TO_COMMAND(TOOL_VIEW, KeyCode::KEY_Q, false, false, false);
	ASSIGN_KEY_TO_COMMAND(TOOL_MOVE, KeyCode::KEY_W, false, false, false);
	ASSIGN_KEY_TO_COMMAND(TOOL_ROTATE, KeyCode::KEY_E, false, false, false);
	ASSIGN_KEY_TO_COMMAND(TOOL_SCALE, KeyCode::KEY_R, false, false, false);

	ASSIGN_KEY_TO_COMMAND(TOOL_PIVOT_ORIENTATION, KeyCode::KEY_X, false, false, false);
}

NS_IGE_END