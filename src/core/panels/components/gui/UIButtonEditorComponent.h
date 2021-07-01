#ifndef __UI_BUTTON_EDITOR_COMPONENT_H_
#define __UI_BUTTON_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class UIButtonEditorComponent : public EditorComponent
{
public:
	UIButtonEditorComponent();
	virtual ~UIButtonEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawUIButton();
protected:
	std::shared_ptr<Group> m_uiButtonGroup = nullptr;


};

NS_IGE_END

#endif //__UI_BUTTON_EDITOR_COMPONENT_H_