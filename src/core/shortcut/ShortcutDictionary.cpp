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

const char* ShortcutDictionary::ASSET_RELOAD_SOURCE = "Tools/Reload Source";

const char* ShortcutDictionary::EDIT_SCENE_OBJECT_SELECTED = "Edit/Scene Object Selected";
const char* ShortcutDictionary::DELETE_SCENE_OBJECT_SELECTED = "Edit/Delete Object Selected";
const char* ShortcutDictionary::COPY_SCENE_OBJECT_SELECTED = "Edit/Copy Object Selected";
const char* ShortcutDictionary::PASTE_SCENE_OBJECT_SELECTED = "Edit/Paste Object Selected";
const char* ShortcutDictionary::DUPLICATE_SCENE_OBJECT_SELECTED = "Edit/Duplicate Object Selected";
const char* ShortcutDictionary::EDIT_UNDO = "Edit/Undo (Ctrl + Z)";
const char* ShortcutDictionary::EDIT_REDO = "Edit/Redo (Ctrl + Y)";

const char* ShortcutDictionary::FILE_NEW_PROJECT_SELECTED = "File/New Project";
const char* ShortcutDictionary::FILE_OPEN_PROJECT_SELECTED = "File/Open Project";
const char* ShortcutDictionary::FILE_NEW_SCENE_SELECTED = "File/New Scene";
const char* ShortcutDictionary::FILE_SAVE_SCENE_SELECTED = "File/Save Scene";
const char* ShortcutDictionary::FILE_SAVE_SCENE_AS_SELECTED = "File/Save Scene As";

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

	ASSIGN_COMMAND_TO_DICT(ASSET_RELOAD_SOURCE, EditorUtils::Assets_ToggleReloadSource);
	
}

void ShortcutDictionary::assignDefaultShortcut() {
	ASSIGN_KEY_TO_COMMAND(TOOL_VIEW, KeyCode::KEY_Q, false, false, false);
	ASSIGN_KEY_TO_COMMAND(TOOL_MOVE, KeyCode::KEY_W, false, false, false);
	ASSIGN_KEY_TO_COMMAND(TOOL_ROTATE, KeyCode::KEY_E, false, false, false);
	ASSIGN_KEY_TO_COMMAND(TOOL_SCALE, KeyCode::KEY_R, false, false, false);

	ASSIGN_KEY_TO_COMMAND(TOOL_PIVOT_ORIENTATION, KeyCode::KEY_X, false, false, false);

	ASSIGN_KEY_TO_COMMAND(ASSET_RELOAD_SOURCE, KeyCode::KEY_R, false, true, false);
}

NS_IGE_END