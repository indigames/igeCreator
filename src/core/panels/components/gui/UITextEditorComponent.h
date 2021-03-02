#ifndef __UI_TEXT_EDITOR_COMPONENT_H_
#define __UI_TEXT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class UITextEditorComponent : public EditorComponent
{
public:
	UITextEditorComponent();
	~UITextEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawUIText();
protected:
	std::shared_ptr<Group> m_uiTextGroup = nullptr;


};

NS_IGE_END

#endif //__UI_TEXT_EDITOR_COMPONENT_H_