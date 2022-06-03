#pragma once
#ifndef __SHORTCUT_DICTIONARY_H__
#define __SHORTCUT_DICTIONARY_H__

#include <utils/PyxieHeaders.h>
#include "core/Macros.h"

USING_NS_PYXIE

NS_IGE_BEGIN

class ShortcutDictionary 
{
public:
	//! Tool
	const static char* TOOL_VIEW;
	const static char* TOOL_MOVE;
	const static char* TOOL_ROTATE;
	const static char* TOOL_SCALE;
	const static char* TOOL_PIVOT_POSITION;
	const static char* TOOL_PIVOT_ORIENTATION;

	//! Edit
	const static char* EDIT_SCENE_OBJECT_SELECTED;
	const static char* DELETE_SCENE_OBJECT_SELECTED;
	const static char* COPY_SCENE_OBJECT_SELECTED;
	const static char* PASTE_SCENE_OBJECT_SELECTED;
	const static char* DUPLICATE_SCENE_OBJECT_SELECTED;
	const static char* EDIT_UNDO;
	const static char* EDIT_REDO;

	//! File
	const static char* FILE_NEW_PROJECT_SELECTED;
	const static char* FILE_OPEN_PROJECT_SELECTED;
	const static char* FILE_NEW_SCENE_SELECTED;
	const static char* FILE_SAVE_SCENE_SELECTED;
	const static char* FILE_SAVE_SCENE_AS_SELECTED;

	static void initShortcuts();

protected:
	static void initDefaultShortcutCommand();
	static void assignDefaultShortcut();
};

NS_IGE_END

#endif // __SHORTCUT_DICTIONARY_H__