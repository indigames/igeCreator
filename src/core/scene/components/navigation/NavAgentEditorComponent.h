#ifndef __NAV_AGENT_EDITOR_COMPONENT_H_
#define __NAV_AGENT_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class NavAgentEditorComponent : public EditorComponent
{
public:
	NavAgentEditorComponent();
	virtual ~NavAgentEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawNavAgent();
protected:
	std::shared_ptr<Group> m_navAgentGroup = nullptr;

	
};

NS_IGE_END

#endif //__NAV_AGENT_EDITOR_COMPONENT_H_