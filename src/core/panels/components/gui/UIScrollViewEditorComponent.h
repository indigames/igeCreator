#ifndef __UI_SCROLL_VIEW_EDITOR_COMPONENT_H_
#define __UI_SCROLL_VIEW_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class UIScrollViewEditorComponent : public EditorComponent
{
public:
	UIScrollViewEditorComponent();
	~UIScrollViewEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawUIScrollView();
protected:
	std::shared_ptr<Group> m_uiScrollViewGroup = nullptr;


};

NS_IGE_END

#endif //__UI_SCROLL_VIEW_EDITOR_COMPONENT_H_