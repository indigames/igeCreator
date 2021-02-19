#ifndef __PHYSIC_MANAGER_EDITOR_COMPONENT_H_
#define __PHYSIC_MANAGER_EDITOR_COMPONENT_H_

#include "core/Macros.h"
#include "core/Ref.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class PhysicManagerEditorComponent : public EditorComponent
{
public:
	PhysicManagerEditorComponent();
	~PhysicManagerEditorComponent();

	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawPhysicManager();
protected:
	std::shared_ptr<Group> m_physicManagerGroup = nullptr;


};

NS_IGE_END

#endif //__PHYSIC_MANAGER_EDITOR_COMPONENT_H_