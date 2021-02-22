#ifndef __NAV_AGENT_EDITOR_COMPONENT_H_
#define __NAV_AGENT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class NavAreaEditorComponent : public EditorComponent
{
public:
	NavAreaEditorComponent();
	~NavAreaEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawNavArea();
protected:
	std::shared_ptr<Group> m_navAreaGroup = nullptr;

	
};

NS_IGE_END

#endif //__NAV_AGENT_EDITOR_COMPONENT_H_