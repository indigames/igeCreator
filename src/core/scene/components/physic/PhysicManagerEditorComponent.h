#ifndef __PHYSIC_MANAGER_EDITOR_COMPONENT_H_
#define __PHYSIC_MANAGER_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class PhysicManagerEditorComponent : public EditorComponent
{
public:
	PhysicManagerEditorComponent();
	virtual ~PhysicManagerEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawPhysicManager();
protected:
	std::shared_ptr<Group> m_physicManagerGroup = nullptr;


};

NS_IGE_END

#endif //__PHYSIC_MANAGER_EDITOR_COMPONENT_H_