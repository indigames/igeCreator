#ifndef __UI_MASK_EDITOR_COMPONENT_H_
#define __UI_MASK_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class UIMaskEditorComponent : public EditorComponent
{
public:
	UIMaskEditorComponent();
	~UIMaskEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawUIMask();
protected:
	std::shared_ptr<Group> m_uiMaskGroup = nullptr;


};

NS_IGE_END

#endif //__UI_MASK_EDITOR_COMPONENT_H_