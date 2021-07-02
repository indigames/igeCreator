#ifndef __UI_SLIDER_EDITOR_COMPONENT_H_
#define __UI_SLIDER_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class UISliderEditorComponent : public EditorComponent
{
public:
	UISliderEditorComponent();
	virtual ~UISliderEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawUISlider();

	void drawMin();
	void drawMax();
	void drawValue();
protected:
	std::shared_ptr<Group> m_uiSliderGroup = nullptr;
	std::shared_ptr<Group> m_uiSliderMinGroup = nullptr;
	std::shared_ptr<Group> m_uiSliderMaxGroup = nullptr;
	std::shared_ptr<Group> m_uiSliderValueGroup = nullptr;

	int m_dirtyFlag = 0;
};

NS_IGE_END

#endif //__UI_SLIDER_EDITOR_COMPONENT_H_