#ifndef __NAV_AGENT_MANAGER_EDITOR_COMPONENT_H_
#define __NAV_AGENT_MANAGER_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class NavAgentManagerEditorComponent : public EditorComponent
{
public:
	NavAgentManagerEditorComponent();
	virtual ~NavAgentManagerEditorComponent();

protected:	
	virtual void onInspectorUpdate() override;
	void drawNavAgentManager();

protected:
	std::shared_ptr<Group> m_navAgentManagerGroup = nullptr;	
};

NS_IGE_END

#endif //__NAV_AGENT_MANAGER_EDITOR_COMPONENT_H_