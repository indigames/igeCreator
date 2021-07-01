#ifndef __NAVIGABLE_EDITOR_COMPONENT_H_
#define __NAVIGABLE_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class NavigableEditorComponent : public EditorComponent
{
public:
	NavigableEditorComponent();
	virtual ~NavigableEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawNavigable();
protected:
	std::shared_ptr<Group> m_navigableGroup = nullptr;

	
};

NS_IGE_END

#endif //__NAVIGABLE_EDITOR_COMPONENT_H_