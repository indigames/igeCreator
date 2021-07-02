#ifndef __CANVAS_EDITOR_COMPONENT_H_
#define __CANVAS_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class CanvasEditorComponent : public EditorComponent
{
public:
	CanvasEditorComponent();
	virtual ~CanvasEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawCanvas();
protected:
	std::shared_ptr<Group> m_canvasGroup = nullptr;


};

NS_IGE_END

#endif //__CANVAS_EDITOR_COMPONENT_H_