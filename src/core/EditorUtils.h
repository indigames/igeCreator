#pragma once

#include <utils/PyxieHeaders.h>
#include "core/Macros.h"

USING_NS_PYXIE

NS_IGE_BEGIN

class EditorUtils {
public:
	static void Tools_View();
	static void Tools_Move();
	static void Tools_Rotate();
	static void Tools_Scale();
	static void Tools_TogglePivotPosition();
	static void Tools_TogglePivotOrientation();

	static void Assets_ToggleReloadSource();
};

NS_IGE_END
